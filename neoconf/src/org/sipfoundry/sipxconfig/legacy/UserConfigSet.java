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
package org.sipfoundry.sipxconfig.legacy;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.sipfoundry.sipxconfig.admin.dialplan.config.Permission;

public class UserConfigSet extends ConfigSet {
    private Set m_users;
    private Document m_profile;

    public Set getUsers() {
        return m_users;
    }

    public void setUsers(Set users) {
        m_users = users;
    }

    /**
     * Retrieves the set of permissions from the config sets
     * 
     * <code>
     * 
     * <PROFILE>
     *  <PERMISSIONS>
     *      <name_of_permission>ENABLE</name_of_permission>
     *      <name_of_permission>DISABLE</name_of_permission>
     *      <name_of_permission>ENABLE</name_of_permission>
     *  </PERMISSIONS>
     * </PROFILE>
     * 
     * </code>
     * 
     * @return Map of Permission->Boolean
     */
    public Map getPermissions() {
        Map allPermissions = new HashMap();
        Document profile = getDocument();
        List permissionNodes = profile.selectNodes("/PROFILE/PERMISSIONS/PERMISSIONS/*");
        for (Iterator i = permissionNodes.iterator(); i.hasNext();) {
            Element permElem = (Element) i.next();
            String status = permElem.getText();
            Boolean enabled = Boolean.valueOf("ENABLE".equalsIgnoreCase(status));
            String name = permElem.getName();
            // remove leading underscore characters before changing to permission name
            Permission permission = Permission.getEnum(name.replaceAll("^_*", ""));
            allPermissions.put(permission, enabled);
        }
        return allPermissions;
    }

    private Document getDocument() {
        if (m_profile == null) {
            try {
                m_profile = DocumentHelper.parseText(getContent());
            } catch (DocumentException e) {
                throw new RuntimeException(e);
            }
        }
        return m_profile;
    }

    /**
     * Checks if the permission is present and enabled in this config set
     * 
     * @param p permission
     * @return true if (both) present and enabled, false otherwise
     */
    public Boolean hasPermission(Permission p) {
        Map perms = getPermissions();
        Boolean enabled = (Boolean) perms.get(p);
        return enabled;
    }

    /**
     * @return null or password in clear text. All non-admin users should have a password
     */
    public String getClearTextPassword() {
        Document profile = getDocument();
        return profile.valueOf("//PROFILE/line1[position()=1]/container/line1_password/text()");
    }
}
