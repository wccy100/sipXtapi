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
package org.sipfoundry.sipxconfig.vendor;

import java.text.MessageFormat;

import org.sipfoundry.sipxconfig.core.CoreDao;
import org.sipfoundry.sipxconfig.core.LogicalPhone;
import org.sipfoundry.sipxconfig.core.Phone;

/**
 * Support for SIP Soft Phone and legacy xpressa
 */
public class SipxPhone implements Phone {

    /** system-wide plugin id for device */
    public static final String HARDPHONE = "xpressa_strongarm_vxworks";

    /** system-wide plugin id for device */
    public static final String SOFTPHONE = "ixpressa_x86_win32";

    private static final Profile[] PROFILES = new Profile[] {
        new Profile("pingtel/{0}/{1}/pinger-config", "x-xpressa-device"),
        new Profile("pingtel/{0}/{1}/user-config", "x-xpressa-user"),
        new Profile("pingtel/{0}/{1}/app-config", "x-xpressa-apps"),
        new Profile("pingtel/{0}/{1}/install-script", "x-xpressa-install")
    };

    private String m_id;
    
    private CoreDao m_dao;
        
    /**
     * XML filename that describes a particular model's definitions
     * 
     * @param model
     * @return filepath to xml file
     */
    public String getDefinitions() {
        return new StringBuffer().append('/').append(m_id).append("-definitions.xml").toString();
    }

    public String getModelId() {
        return m_id;
    }

    public void setModelId(String modelId) {
        m_id = modelId;
    }

    public int getProfileCount() {
        return PROFILES.length;
    }

    public String getProfileFileName(int profileIndex, String macAddress) {
        String profileName = MessageFormat.format(PROFILES[profileIndex].getFilename(), 
                new Object[] {m_id, macAddress});

        return profileName.toLowerCase();
    }
    
    public String getProfileSubscribeToken(int profileIndex) {
        return PROFILES[profileIndex].getSubscribeToken();
    }    

    public String getProfileNotifyUrl(LogicalPhone logicalPhoneTemp, int profileIndexTemp) {
        throw new RuntimeException("getDeviceNotifyUrl not implemented yet");
    }

    public int getProfileSequenceNumber(LogicalPhone logicalPhoneTemp, int profileIndexTemp) {
        throw new RuntimeException("getProfileSequenceNumber not implemented yet");
    }
    
    /**
     * @return Returns the dao.
     */
    public CoreDao getCoreDao() {
        return m_dao;
    }
    
    /**
     * @param dao The dao to set.
     */
    public void setCoreDao(CoreDao dao) {
        m_dao = dao;
    }    
}


class Profile {

    private String m_filename;
    
    private String m_subscribeToken;
    
    Profile(String filename, String token) {
        m_filename = filename;
        m_subscribeToken = token;
    }

    public String getFilename() {
        return m_filename;
    }

    public String getSubscribeToken() {
        return m_subscribeToken;
    }
}
