/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.admin.commserver.imdb;

import java.util.Collections;

import org.custommonkey.xmlunit.XMLTestCase;
import org.dom4j.Document;
import org.dom4j.DocumentFactory;
import org.dom4j.Element;
import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.XmlUnitHelper;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.permission.PermissionManagerImpl;
import org.sipfoundry.sipxconfig.permission.PermissionName;
import org.sipfoundry.sipxconfig.setting.Group;

public class PermissionsTest extends XMLTestCase {

    public void testGenerateEmpty() throws Exception {
        IMocksControl control = EasyMock.createControl();
        CoreContext coreContext = control.createMock(CoreContext.class);
        coreContext.getDomainName();
        control.andReturn("host.company.com");
        coreContext.loadUsers();
        control.andReturn(Collections.EMPTY_LIST);
        control.replay();

        Permissions permissions = new Permissions();
        permissions.setCoreContext(coreContext);

        Document document = permissions.generate();
        org.w3c.dom.Document domDoc = XmlUnitHelper.getDomDoc(document);
        assertXpathEvaluatesTo("permission", "/items/@type", domDoc);
        assertXpathNotExists("/items/item", domDoc);
        control.verify();
    }

    public void testAddUser() throws Exception {
        Document document = DocumentFactory.getInstance().createDocument();
        Element items = document.addElement("items");

        PermissionManagerImpl pm = new PermissionManagerImpl();
        pm.setModelFilesContext(TestHelper.getModelFilesContext());

        User user = new User();
        user.setPermissionManager(pm);
        
        Group g = new Group();
        PermissionName.INTERNATIONAL_DIALING.setEnabled(g, false);
        PermissionName.LONG_DISTANCE_DIALING.setEnabled(g, false);
        PermissionName.TOLL_FREE_DIALING.setEnabled(g, false);
        PermissionName.LOCAL_DIALING.setEnabled(g, true);
        

        user.addGroup(g);
        user.setUserName("goober");

        Permissions permissions = new Permissions();
        permissions.addUser(items, user, "sipx.sipfoundry.org");

        org.w3c.dom.Document domDoc = XmlUnitHelper.getDomDoc(document);
        System.out.println(XmlUnitHelper.asString(document));
        assertXpathEvaluatesTo("sip:goober@sipx.sipfoundry.org", "/items/item/identity", domDoc);
        assertXpathEvaluatesTo("LocalDialing", "/items/item/permission", domDoc);
    }
}
