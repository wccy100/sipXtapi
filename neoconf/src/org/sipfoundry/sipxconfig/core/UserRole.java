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
package org.sipfoundry.sipxconfig.core;

/**
 * Database object
 */
public class UserRole {

    private int m_usrsId;

    private String m_rolesName;

    public int getUsrsId() {
        return m_usrsId;
    }

    public void setUsrsId(int usrsId) {
        m_usrsId = usrsId;
    }

    public String getRolesName() {
        return m_rolesName;
    }

    public void setRolesName(String rolesName) {
        m_rolesName = rolesName;
    }

}
