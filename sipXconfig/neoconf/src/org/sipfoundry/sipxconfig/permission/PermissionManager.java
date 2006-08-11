/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.permission;

import java.util.Collection;


public interface PermissionManager {
    
    Permission getPermission(String name);

    void addCallPermission(Permission permission);

    Collection<Permission> getCallPermissions();

    void removeCallPermissions(Collection<String> permissionNames);
}
