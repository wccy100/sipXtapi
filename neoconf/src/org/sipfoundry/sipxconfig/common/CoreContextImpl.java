/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.common;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.hibernate.Criteria;
import org.hibernate.Session;
import org.hibernate.criterion.Criterion;
import org.hibernate.criterion.Example;
import org.hibernate.criterion.MatchMode;
import org.hibernate.criterion.Restrictions;
import org.sipfoundry.sipxconfig.common.event.DaoEventListener;
import org.sipfoundry.sipxconfig.common.event.DaoEventPublisher;
import org.sipfoundry.sipxconfig.setting.Group;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingDao;
import org.springframework.context.ApplicationEvent;
import org.springframework.context.ApplicationListener;
import org.springframework.orm.hibernate3.HibernateCallback;
import org.springframework.orm.hibernate3.support.HibernateDaoSupport;

public class CoreContextImpl extends HibernateDaoSupport implements CoreContext,
        ApplicationListener, DaoEventListener {

    private static final String USER_GROUP_RESOURCE_ID = "user";
    private static final String USERNAME_PROP_NAME = "userName";

    private String m_authorizationRealm;

    private String m_domainName;

    private SettingDao m_settingDao;

    private Setting m_userSettingModel;
    
    private DaoEventPublisher m_daoEventPublisher;

    public CoreContextImpl() {
        super();
    }

    public String getAuthorizationRealm() {
        return m_authorizationRealm;
    }

    public void setAuthorizationRealm(String authorizationRealm) {
        m_authorizationRealm = authorizationRealm;
    }

    public String getDomainName() {
        return m_domainName;
    }

    public void setDomainName(String domainName) {
        m_domainName = domainName;
    }

    public void saveUser(User user) {
        getHibernateTemplate().saveOrUpdate(user);
    }

    public void deleteUser(User user) {        
        getHibernateTemplate().delete(user);
    }

    public void deleteUsers(Collection userIds) {
        if (userIds.isEmpty()) {
            // no users to delete => nothing to do
            return;
        }
        List users = new ArrayList(userIds.size());
        for (Iterator i = userIds.iterator(); i.hasNext();) {
            Integer id = (Integer) i.next();
            User user = loadUser(id);
            users.add(user);
            m_daoEventPublisher.publishDelete(user);
        }
        getHibernateTemplate().deleteAll(users);
    }

    public User loadUser(Integer id) {
        User user = (User) getHibernateTemplate().load(User.class, id);

        return user;
    }

    public User loadUserByUserName(String userName) {
        return loadUserByUniqueProperty(USERNAME_PROP_NAME, userName);
    }

    private User loadUserByUniqueProperty(String propName, String propValue) {
        final Criterion expression = Restrictions.eq(propName, propValue);

        HibernateCallback callback = new HibernateCallback() {
            public Object doInHibernate(Session session) {
                Criteria criteria = session.createCriteria(User.class).add(expression);
                return criteria.list();
            }
        };
        List users = getHibernateTemplate().executeFind(callback);
        User user = (User) requireOneOrZero(users, expression.toString());

        return user;
    }
    
    public User loadUserByAlias(String alias) {
        return loadUserByNamedQueryAndNamedParam("userByAlias", "alias", alias);
    }
    
    public User loadUserByUserNameOrAlias(String userNameOrAlias) {
        return loadUserByNamedQueryAndNamedParam(
                "userByNameOrAlias", "userNameOrAlias", userNameOrAlias);
    }

    private User loadUserByNamedQueryAndNamedParam(String queryName, String paramName, Object value) {
        Collection usersColl = getHibernateTemplate().findByNamedQueryAndNamedParam(
                queryName, paramName, value);
        Set users = new HashSet(usersColl);     // eliminate duplicates
        if (users.size() > 1) {
            throw new IllegalStateException(
                    "The database has more than one user matching the query "
                    + queryName + ", paramName = " + paramName + ", value = " + value);
        }
        User user = null;
        if (users.size() > 0) {
            user = (User) users.iterator().next();
        }
        return user;       
    }
    
    public List loadUserByTemplateUser(User template) {
        final Example example = Example.create(template);
        example.setPropertySelector(NotNullOrBlank.INSTANCE);
        example.enableLike(MatchMode.START);
        example.excludeProperty("id");

        HibernateCallback callback = new HibernateCallback() {
            public Object doInHibernate(Session session) {
                Criteria criteria = session.createCriteria(User.class).add(example);
                return criteria.list();
            }
        };
        return getHibernateTemplate().executeFind(callback);
    }

    public List loadUsers() {
        return getHibernateTemplate().loadAll(User.class);
    }

    public Object load(Class c, Integer id) {
        return getHibernateTemplate().load(c, id);
    }

    /**
     * Catch database corruption errors where more than one record exists. In general fields should
     * have unique indexes set up to protect against this. This method is created as a safe check
     * only, there have been not been any experiences of corrupt data to date.
     * 
     * @param c
     * @param query
     * 
     * @return first item from the collection
     * @throws IllegalStateException if more than one item in collection.
     */
    public static Object requireOneOrZero(Collection c, String query) {
        if (c.size() > 2) {
            // DatabaseCorruptionException ?
            // TODO: move error string construction to new UnexpectedQueryResult(?) class, enable
            // localization
            StringBuffer error = new StringBuffer().append("read ").append(c.size()).append(
                    " and expected zero or one. query=").append(query);
            throw new IllegalStateException(error.toString());
        }
        Iterator i = c.iterator();

        return (i.hasNext() ? c.iterator().next() : null);
    }

    public void clear() {
        Collection c = getHibernateTemplate().find("from User");
        getHibernateTemplate().deleteAll(c);
    }

    public void setDaoEventPublisher(DaoEventPublisher daoEventPublisher) {
        m_daoEventPublisher = daoEventPublisher;
    }
    
    public void onApplicationEvent(ApplicationEvent event) {
        if (event instanceof InitializationTask) {
            InitializationTask task = (InitializationTask) event;
            if (task.getTask().equals("admin-group-and-user")) {
                createAdminGroupAndInitialUserTask();
            }
        }
    }

    /**
     * Temporary hack: create a superadmin user with an empty password. We will remove this hack
     * before the product ships, and provide a bootstrap page instead so that if the product comes
     * up and there are no users, then the first user can be created.
     */
    public void createAdminGroupAndInitialUserTask() {
        Group adminGroup = new Group();
        adminGroup.setName("administrators"); // nothing special about name
        adminGroup.setResource(User.GROUP_RESOURCE_ID);
        adminGroup.setDescription("Users with superadmin privileges");
        Permission.SUPERADMIN.setEnabled(adminGroup, true);
        Permission.TUI_CHANGE_PIN.setEnabled(adminGroup, false);

        m_settingDao.saveGroup(adminGroup);

        // using superadmin name not to disrupt existing customers
        // can be anything
        String superadmin = "superadmin";
        User admin = loadUserByUserName(superadmin);
        if (admin == null) {
            admin = new User();
            admin.setUserName(superadmin);
            // Note: previously this hack set the pintoken to 'password', relying on another hack
            // that allowed the password and pintoken to be the same. That hack is gone so setting
            // the pintoken to 'password' would no longer work because the password would then be
            // the inverse hash of 'password' rather than 'password'.
            admin.setPintoken("");
        }

        admin.addGroup(adminGroup);
        saveUser(admin);
    }

    public void setSettingDao(SettingDao settingDao) {
        m_settingDao = settingDao;
    }

    public List getUserGroups() {
        return m_settingDao.getGroups(USER_GROUP_RESOURCE_ID);
    }

    public Setting getUserSettingsModel() {
        // return copy so original model stays intact
        return m_userSettingModel.copy();
    }

    public void setUserSettingModel(Setting userSettingModel) {
        m_userSettingModel = userSettingModel;
    }

    public List getUserAliases() {
        List aliases = new ArrayList();
        List users = loadUsers();
        for (Iterator i = users.iterator(); i.hasNext();) {
            User user = (User) i.next();
            aliases.addAll(user.getAliasMappings(m_domainName));
        }
        return aliases;
    }

    public Collection getGroupMembers(Group group) {
        Collection users = getHibernateTemplate().findByNamedQueryAndNamedParam(
                "userGroupMembers", "groupId", group.getId());
        return users;
    }

    public void onDelete(Object entity) {
        if (entity instanceof Group) {
            Group group = (Group) entity;
            getHibernateTemplate().update(group);
            if (User.GROUP_RESOURCE_ID.equals(group.getResource())) {
                Collection users = getGroupMembers(group);
                Iterator iusers = users.iterator();
                while (iusers.hasNext()) {
                    User user = (User) iusers.next();
                    Object[] ids = new Object[] {
                        group.getId()
                    };
                    DataCollectionUtil.removeByPrimaryKey(user.getGroups(), ids);
                    saveUser(user);
                }
            }
        }
    }

    public void onSave(Object entity_) {
    }
    
}
