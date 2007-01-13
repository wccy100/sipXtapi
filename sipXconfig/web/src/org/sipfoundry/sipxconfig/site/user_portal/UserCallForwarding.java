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
package org.sipfoundry.sipxconfig.site.user_portal;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.tapestry.annotations.InjectObject;
import org.apache.tapestry.event.PageBeginRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.sipfoundry.sipxconfig.admin.forwarding.CallSequence;
import org.sipfoundry.sipxconfig.admin.forwarding.ForwardingContext;
import org.sipfoundry.sipxconfig.admin.forwarding.Ring;
import org.sipfoundry.sipxconfig.common.BeanWithId;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.permission.PermissionName;

/**
 * UserCallForwarding
 */
public abstract class UserCallForwarding extends UserBasePage implements PageBeginRenderListener {
    public static final String PAGE = "UserCallForwarding";
    private static final String ACTION_ADD = "add";

    @InjectObject(value = "spring:forwardingContext")
    public abstract ForwardingContext getForwardingContext();

    public abstract String getAction();

    public abstract List getRings();

    public abstract void setRings(List rings);

    public void pageBeginRender(PageEvent event) {
        if (getRings() != null) {
            return;
        }

        super.pageBeginRender(event);

        List rings = createDetachedRingList(getCallSequence());
        setRings(rings);
    }

    /**
     * Create list of rings that is going to be stored in session.
     * 
     * The list is a clone of the list kept by current call sequence, ring objects do not have
     * valid ids and their call sequence field is set to null.
     */
    private List createDetachedRingList(CallSequence callSequence) {
        List rings = callSequence.getRings();
        List list = new ArrayList();
        for (Iterator i = rings.iterator(); i.hasNext();) {
            BeanWithId ring = (BeanWithId) i.next();
            Ring dup = (Ring) ring.duplicate();
            dup.setCallSequence(null);
            list.add(dup);
        }
        return list;
    }

    private CallSequence getCallSequence() {
        ForwardingContext forwardingContext = getForwardingContext();
        Integer userId = getUserId();
        return forwardingContext.getCallSequenceForUserId(userId);
    }

    public void submit() {
        if (!TapestryUtils.isValid(this)) {
            // do nothing on errors
            return;
        }
        if (ACTION_ADD.equals(getAction())) {
            getRings().add(new Ring());
        }
    }

    public void commit() {
        if (!TapestryUtils.isValid(this)) {
            // do nothing on errors
            return;
        }
        CallSequence callSequence = getCallSequence();
        callSequence.clear();
        callSequence.insertRings(getRings());
        getForwardingContext().saveCallSequence(callSequence);
    }

    public void deleteRing(int position) {
        getRings().remove(position);
    }

    public String getFirstCallMsg() {
        Object[] params = {
            getUser().getUserName()
        };
        return getMessages().format("msg.first", params);
    }

    public boolean getHasVoiceMail() {
        return getUser().hasPermission(PermissionName.VOICEMAIL);
    }
}
