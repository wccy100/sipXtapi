/*
 * 
 * 
 * Copyright (C) 2007 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2007 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.device;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;

public abstract class AbstractProfileGenerator implements ProfileGenerator {
    private ProfileLocation m_profileLocation;

    public void setProfileLocation(ProfileLocation profileLocation) {
        m_profileLocation = profileLocation;
    }

    public void generate(ProfileContext context, String templateFileName, String outputFileName) {
        generate(context, templateFileName, null, outputFileName);
    }

    public void generate(ProfileContext context, String templateFileName, ProfileFilter filter,
            String outputFileName) {
        if (outputFileName == null) {
            return;
        }

        OutputStream wtr = m_profileLocation.getOutput(outputFileName);
        try {
            if (filter == null) {
                generateProfile(context, templateFileName, wtr);
            } else {
                ByteArrayOutputStream unformatted = new ByteArrayOutputStream();
                generateProfile(context, templateFileName, unformatted);
                unformatted.close();
                filter.copy(new ByteArrayInputStream(unformatted.toByteArray()), wtr);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            IOUtils.closeQuietly(wtr);
        }
    }

    public void remove(String outputFileName) {
        if (StringUtils.isNotEmpty(outputFileName)) {
            m_profileLocation.removeProfile(outputFileName);
        }
    }

    protected abstract void generateProfile(ProfileContext context, String templateFileName,
            OutputStream out) throws IOException;
}
