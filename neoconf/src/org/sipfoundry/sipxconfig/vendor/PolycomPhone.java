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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.VelocityEngine;
import org.sipfoundry.sipxconfig.phone.GenericPhone;
import org.sipfoundry.sipxconfig.phone.Organization;
import org.sipfoundry.sipxconfig.phone.PhoneContext;
import org.sipfoundry.sipxconfig.phone.PhoneDao;
import org.sipfoundry.sipxconfig.phone.SettingSet;

/**
 * Support for Polycom 300, 400, and 500 series phones and model 3000
 * conference phone
 */
public class PolycomPhone extends GenericPhone {

    /** basic model */
    public static final String MODEL_300 = "polycom300";

    /** standard model */
    public static final String MODEL_500 = "polycom500";

    /** deluxe model */
    public static final String MODEL_600 = "polycom600";

    /** conference phone */
    public static final String MODEL_3000 = "polycom3000";

    private String m_id;

    private File m_tftpRoot;
    
    private VelocityEngine m_velocityEngine;

    public VelocityEngine getVelocityEngine() {
        return m_velocityEngine;
    }
    
    public void setVelocityEngine(VelocityEngine velocityEngine) {
        m_velocityEngine = velocityEngine;
    }
    
    public String getTftpRoot() {
        return m_tftpRoot.getName();
    }

    public void setTftpRoot(String tftpRoot) {
        m_tftpRoot = new File(tftpRoot);
    }

    public String getModelId() {
        return m_id;
    }

    public String getDisplayLabel() {
        return "Polycom SoundPoint IP 300";
    }

    public void setModelId(String id) {
        m_id = id;
    }
    
    public void initialize() {
        SettingSet settings = getEndpoint().getSettings();
        if (settings == null) {
            // TODO not sure i want to do this
            settings = new SettingSet();
            getEndpoint().setSettings(settings);
        }
        PhoneContext context = getPhoneContext();
        PhoneDao dao = context.getPhoneDao();
        Organization org = dao.loadRootOrganization();
        
        settings.setDefault("network/tftpServer", org.getDnsDomain());
        
        if (!m_tftpRoot.exists()) {
            if (!m_tftpRoot.mkdirs()) {
                throw new RuntimeException("Could not create TFTP root directory " + m_tftpRoot.getPath());
            }
        }
    }

    public void generateProfiles() throws IOException {
        Template template;
        String templateFile = "polycom/phone1.cfg";
        try {
            template = m_velocityEngine.getTemplate(templateFile);
        } catch (Exception e) {
            throw new RuntimeException("Error creating velocity template " + templateFile, e);
        }
        
        // PERFORMANCE: depending on how resource intensive the above code is
        // try to reuse the template objects for subsequent profile
        // generations

        VelocityContext velocityContext = new VelocityContext();
        velocityContext.put("phone", this);

        FileWriter wtr = null;
        try {
            wtr = new FileWriter(new File(m_tftpRoot, getPhoneConfigFilename()));
            template.merge(velocityContext, wtr);
        } catch (Exception e) {
            throw new RuntimeException("Error processing velocity template " + templateFile, e);
        } finally {
            if (wtr != null) {
                wtr.close();
            }
        }
    }
    
    public String getPhoneConfigFilename() {
        // TODO: if using TFTP, have to put sequence number into config file
        // per polycom documentation
        return getEndpoint().getSerialNumber() + ".cfg";
    }

    public InputStream getPhoneConfigFile() throws IOException {
        return new FileInputStream(new File(m_tftpRoot, getPhoneConfigFilename()));
    }
}
