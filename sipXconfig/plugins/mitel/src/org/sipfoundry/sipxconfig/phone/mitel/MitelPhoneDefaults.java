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
package org.sipfoundry.sipxconfig.phone.mitel;

import org.sipfoundry.sipxconfig.device.DeviceDefaults;
import org.sipfoundry.sipxconfig.setting.SettingEntry;

public class MitelPhoneDefaults {

    private DeviceDefaults m_defaults;

    public MitelPhoneDefaults(DeviceDefaults defaults) {
        m_defaults = defaults;
    }

    @SettingEntry(path = "time/timezone")
    public int getTimezone() {
        int tzhrs = m_defaults.getTimeZone().getOffset() / 3600;
        // start counting from date change line
        return tzhrs + 12;
    }

    @SettingEntry(path = "time/adjust_dst")
    public boolean getAdjustDst() {
        return m_defaults.getTimeZone().getDstActive();
    }
}
