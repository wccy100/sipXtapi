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
package org.sipfoundry.sipxconfig.setting;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Config file based storage implementation. Setting.profileName property is used to identify
 * configuration file Files are read using standard java Properties and written by
 * ConfigFileStorage
 * 
 * @see ConfigFileStorage
 */
public class ConfigFileStorage implements Storage {
    private static final Log LOG = LogFactory.getLog(ConfigFileStorage.class);
    private Map m_file2properties = new HashMap();
    private String m_configDirectory;
        
    public ConfigFileStorage(String configDirectory) {
        m_configDirectory = configDirectory;
    }

    public String getValue(Setting setting) {
        SettingValue2 v = getSettingValue(setting);
        return v == null ? null : v.getValue();
    }

    public void setValue(Setting setting, String value) {
        setSettingValue(setting, new SettingValueImpl(value), null);
    }

    public SettingValue2 getSettingValue(Setting setting) {
        SettingValue2 value = null;
        try {
            Properties properties = loadForFile(setting);
            String svalue = properties.getProperty(setting.getName());
            if (svalue != null) {
                value = new SettingValueImpl(svalue.trim());
            }
        } catch (IOException e) {
            LOG.warn("cannot get config file value", e);
        }
        return value;
    }

    public void setSettingValue(Setting setting, SettingValue2 value, SettingValue2 defaultValue) {
        try {
            Properties properties = loadForFile(setting);
            properties.put(setting.getName(), StringUtils.defaultString(value.getValue()));
        } catch (IOException e) {
            LOG.warn("cannot set config file value", e);
        }
    }

    /**
     * Remove is called when setting is set to default value.
     */
    public void revertSettingToDefault(Setting setting) {
        try {
            Properties properties = loadForFile(setting);
            String value = StringUtils.defaultString(setting.getValue());
            properties.put(setting.getName(), value);
        } catch (IOException e) {
            LOG.warn("cannot revert config file value", e);
        }
    }

    private Properties loadForFile(Setting setting) throws IOException {
        String filename = setting.getProfileName();
        Properties properties = (Properties) m_file2properties.get(filename);
        if (null != properties) {
            return properties;
        }
        File file = new File(m_configDirectory, filename);
        FileInputStream stream = new FileInputStream(file);
        properties = new Properties();
        properties.load(stream);
        IOUtils.closeQuietly(stream);
        m_file2properties.put(filename, properties);
        return properties;
    }

    public void flush() throws IOException {
        for (Iterator i = m_file2properties.entrySet().iterator(); i.hasNext();) {
            Map.Entry entry = (Map.Entry) i.next();
            String fileName = (String) entry.getKey();
            Properties props = (Properties) entry.getValue();
            File file = new File(m_configDirectory, fileName);
            ConfigFileWriter writer = new ConfigFileWriter(file);
            writer.store(props);
        }
    }
}
