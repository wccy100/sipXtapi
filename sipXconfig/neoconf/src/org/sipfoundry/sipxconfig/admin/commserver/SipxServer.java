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
package org.sipfoundry.sipxconfig.admin.commserver;

import java.io.IOException;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.admin.commserver.imdb.DataSet;
import org.sipfoundry.sipxconfig.admin.forwarding.AliasMapping;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.SipUri;
import org.sipfoundry.sipxconfig.setting.BeanWithSettings;
import org.sipfoundry.sipxconfig.setting.ConfigFileStorage;
import org.sipfoundry.sipxconfig.setting.Setting;

public class SipxServer extends BeanWithSettings implements Server, AliasProvider {
    private static final String DOMAIN_NAME = "domain/SIPXCHANGE_DOMAIN_NAME";
    private static final String SIP_REGISTRAR_DOMAIN_ALIASES = "domain/SIP_REGISTRAR_DOMAIN_ALIASES";
    private static final String PRESENCE_SIGN_IN_CODE = "presence/SIP_PRESENCE_SIGN_IN_CODE";
    private static final String PRESENCE_SIGN_OUT_CODE = "presence/SIP_PRESENCE_SIGN_OUT_CODE";
    private static final String PRESENCE_SERVER_SIP_PORT = "presence/PRESENCE_SERVER_SIP_PORT";
    // note: the name of the setting is misleading - this is actually full host name not just a
    // domain name
    private static final String PRESENCE_SERVER_LOCATION = "presence/SIP_PRESENCE_DOMAIN_NAME";
    private static final String PRESENCE_API_PORT = "presence/SIP_PRESENCE_HTTP_PORT";

    // alternative way of getting park server address
    private static final String PARK_ADDRESS = "park/SIP_REGISTRAR_PARK_SERVER";

    private String m_configDirectory;
    private ConfigFileStorage m_storage;
    private SipxReplicationContext m_sipxReplicationContext;
    private CoreContext m_coreContext;
    private String m_mohUser;

    public void setSipxReplicationContext(SipxReplicationContext sipxReplicationContext) {
        m_sipxReplicationContext = sipxReplicationContext;
    }

    public void setCoreContext(CoreContext coreContext) {
        m_coreContext = coreContext;
    }

    public void setMohUser(String mohUser) {
        m_mohUser = mohUser;
    }

    protected Setting loadSettings() {
        return getModelFilesContext().loadModelFile("server.xml", "commserver");
    }

    /**
     * Still need to call <code>applySettings</code> save to send to disk
     */
    public void setDomainName(String domainName) {
        setSettingValue(DOMAIN_NAME, domainName);
    }

    public void applySettings() {
        try {
            m_storage.flush();
            handlePossiblePresenceServerChange();
        } catch (IOException e) {
            // TODO: catch and report as User Exception
            throw new RuntimeException(e);
        }
    }

    public void resetSettings() {
        m_storage.reset();
    }

    private void handlePossiblePresenceServerChange() {
        // TODO: in reality only need to do that if sing-in/sign-out code changed
        m_sipxReplicationContext.generate(DataSet.ALIAS);
    }

    public void setConfigDirectory(String configDirectory) {
        m_configDirectory = configDirectory;
        m_storage = new ConfigFileStorage(m_configDirectory);
        setValueStorage(m_storage);
    }

    public Collection getAliasMappings() {
        Collection aliases = new ArrayList();
        String domainName = m_coreContext.getDomainName();
        int presencePort = getPresenceServerPort();
        String signInCode = getSettingValue(PRESENCE_SIGN_IN_CODE);
        String signOutCode = getSettingValue(PRESENCE_SIGN_OUT_CODE);
        String presenceServer = getPresenceServerLocation();

        aliases.add(createPresenceAliasMapping(signInCode.trim(), domainName, presenceServer,
                presencePort));
        aliases.add(createPresenceAliasMapping(signOutCode.trim(), domainName, presenceServer,
                presencePort));

        return aliases;
    }

    public String getPresenceServiceUri() {
        Object[] params = new Object[] {
            getPresenceServerLocation(), String.valueOf(getPresenceServerApiPort())
        };
        return MessageFormat.format("http://{0}:{1}/RPC2", params);
    }

    private String getPresenceServerLocation() {
        return getSettingValue(PRESENCE_SERVER_LOCATION);
    }

    private int getPresenceServerApiPort() {
        return (Integer) getSettingTypedValue(PRESENCE_API_PORT);
    }

    private int getPresenceServerPort() {
        return (Integer) getSettingTypedValue(PRESENCE_SERVER_SIP_PORT);
    }

    private AliasMapping createPresenceAliasMapping(String code, String domainName,
            String presenceServer, int port) {
        AliasMapping mapping = new AliasMapping();
        mapping.setIdentity(AliasMapping.createUri(code, domainName));
        mapping.setContact(SipUri.format(code, presenceServer, port));
        return mapping;
    }

    public String getPresenceServerUri() {
        return SipUri.format(getPresenceServerLocation(), getPresenceServerPort());
    }

    public String getMusicOnHoldUri() {
        String parkAddress = (String) getSettingTypedValue(PARK_ADDRESS);
        return SipUri.format(m_mohUser, parkAddress, false);
    }

    public void setRegistrarDomainAliases(Collection<String> aliases) {
        Setting setting = getSettings().getSetting(SIP_REGISTRAR_DOMAIN_ALIASES);
        List<String> allAliases = new ArrayList<String>();
        allAliases.add(setting.getDefaultValue());
        if (aliases != null) {
            allAliases.addAll(aliases);
        }
        String aliasesString = StringUtils.join(allAliases.iterator(), ' ');
        setting.setValue(aliasesString);
    }

    @Override
    public void initialize() {
    }
}
