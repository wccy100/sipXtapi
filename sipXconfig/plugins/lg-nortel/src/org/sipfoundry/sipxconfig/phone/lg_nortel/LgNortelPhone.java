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
package org.sipfoundry.sipxconfig.phone.lg_nortel;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.device.DeviceDefaults;
import org.sipfoundry.sipxconfig.device.ProfileContext;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.LineInfo;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.speeddial.SpeedDial;

public class LgNortelPhone extends Phone {
    public static final String BEAN_ID = "lg-nortel";

    public LgNortelPhone() {
        super(BEAN_ID);
    }

    public LgNortelPhone(LgNortelModel model) {
        super(model);
    }

    @Override
    public String getPhoneTemplate() {
        return "lg-nortel/mac.cfg.vm";
    }

    @Override
    public void initializeLine(Line line) {
        DeviceDefaults phoneDefaults = getPhoneContext().getPhoneDefaults();
        LgNortelLineDefaults defaults = new LgNortelLineDefaults(phoneDefaults, line);
        line.addDefaultBeanSettingHandler(defaults);
    }

    @Override
    public void initialize() {
        DeviceDefaults phoneDefaults = getPhoneContext().getPhoneDefaults();
        int lines = getLines().size();
        LgNortelPhoneDefaults defaults = new LgNortelPhoneDefaults(phoneDefaults, lines);
        addDefaultBeanSettingHandler(defaults);
    }

    @Override
    protected ProfileContext createContext() {
        SpeedDial speedDial = getPhoneContext().getSpeedDial(this);
        return new LgNortelProfileContext(this, speedDial, getPhoneTemplate());
    }

    static class LgNortelProfileContext extends ProfileContext {
        private SpeedDial m_speeddial;

        LgNortelProfileContext(LgNortelPhone phone, SpeedDial speeddial, String profileTemplate) {
            super(phone, profileTemplate);
            m_speeddial = speeddial;
        }

        public Map<String, Object> getContext() {
            Map<String, Object> context = super.getContext();
            Collection buttons = Collections.EMPTY_LIST;
            if (m_speeddial != null) {
                buttons = m_speeddial.getButtons();
            }
            context.put("speeddials", buttons);

            int speeddialOffset = 0;
            Collection lines = ((Phone) getDevice()).getLines();
            if (lines != null) {
                speeddialOffset = lines.size();
            }
            context.put("speeddial_offset", speeddialOffset);

            return context;
        }
    }

    @Override
    public String getPhoneFilename() {
        return StringUtils.defaultString(getSerialNumber()).toUpperCase();
    }

    @Override
    protected LineInfo getLineInfo(Line line) {
        LineInfo lineInfo = LgNortelLineDefaults.getLineInfo(line);
        return lineInfo;
    }

    @Override
    protected void setLineInfo(Line line, LineInfo lineInfo) {
        LgNortelLineDefaults.setLineInfo(line, lineInfo);
    }

    @Override
    public void restart() {
        sendCheckSyncToFirstLine();
    }
}