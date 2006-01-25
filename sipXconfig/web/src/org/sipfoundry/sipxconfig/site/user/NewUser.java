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
package org.sipfoundry.sipxconfig.site.user;

import org.apache.commons.lang.RandomStringUtils;
import org.apache.tapestry.IActionListener;
import org.apache.tapestry.IComponent;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.components.FormActions;
import org.sipfoundry.sipxconfig.components.PageWithCallback;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.site.admin.ExtensionPoolsPage;

public abstract class NewUser extends PageWithCallback implements PageRenderListener {

    public static final String PAGE = "NewUser";
    private static final int SIP_PASSWORD_LEN = 8;

    public abstract CoreContext getCoreContext();

    public abstract User getUser();

    public abstract void setUser(User user);

    public IActionListener getCommitListener() {
        return new IActionListener() {
            public void actionTriggered(IComponent component, IRequestCycle cycle) {
                if (TapestryUtils.isValid(NewUser.this)) {
                    // Save the user
                    CoreContext core = getCoreContext();
                    User user = getUser();
                    core.saveUser(user);

                    // On saving the user, transfer control to edituser page.
                    FormActions buttons = (FormActions) component;
                    if (buttons.wasApplyPressedAndNotCancel()) {
                        EditUser edit = (EditUser) cycle.getPage(EditUser.PAGE);
                        edit.setUserId(user.getId());
                        cycle.activate(edit);
                    }
                }
            }
        };
    }

    public void extensionPools(IRequestCycle cycle) {
        ExtensionPoolsPage poolsPage = (ExtensionPoolsPage) cycle.getPage(
                ExtensionPoolsPage.PAGE);
        poolsPage.activatePageWithCallback(PAGE, cycle);        
    }

    public void pageBeginRender(PageEvent event) {
        User user = getUser();
        if (user == null) {
            user = new User();
            user.setSipPassword(RandomStringUtils.randomAlphanumeric(SIP_PASSWORD_LEN));
            setUser(user);
        }
    }
}
