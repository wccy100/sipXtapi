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
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.io.Serializable;
import java.util.Collection;
import java.util.List;

import org.sipfoundry.sipxconfig.admin.dialplan.config.ConfigGenerator;
import org.sipfoundry.sipxconfig.alias.AliasOwner;
import org.sipfoundry.sipxconfig.common.DataObjectSource;
import org.sipfoundry.sipxconfig.setting.Group;

/**
 * DialPlanContext
 */
public interface DialPlanContext extends DataObjectSource, AliasOwner {

    public static final String CONTEXT_BEAN_NAME = "dialPlanContext";

    public abstract void clear();

    public abstract ConfigGenerator generateDialPlan();

    public abstract void activateDialPlan();

    public abstract ConfigGenerator getGenerator();

    public abstract DialingRuleFactory getRuleFactory();

    public abstract void storeAutoAttendant(AutoAttendant attendant);

    public abstract void deleteAutoAttendant(AutoAttendant attendant, String scriptsDir);

    public abstract AutoAttendant getOperator();

    public abstract AutoAttendant getAutoAttendant(Integer id);

    public abstract List getAutoAttendants();

    public abstract void deleteAutoAttendantsByIds(Collection attendantsIds, String scriptsDir);

    public abstract void specialAutoAttendantMode(boolean enabled, AutoAttendant attendant);

    public abstract void removeGateways(Collection gatewaysIds);

    public void storeRule(DialingRule rule);

    public void addRule(int position, DialingRule rule);

    public List getRules();

    public DialingRule getRule(Integer id);

    public void deleteRules(Collection selectedRows);

    public void duplicateRules(Collection selectedRows);

    public void moveRules(Collection selectedRows, int step);

    public List getGenerationRules();

    public List getAttendantRules();

    public void resetToFactoryDefault();

    public boolean isDialPlanEmpty();

    public String getVoiceMail();

    public abstract void applyEmergencyRouting();

    public abstract void storeEmergencyRouting(EmergencyRouting emergencyRouting);

    public abstract EmergencyRouting getEmergencyRouting();

    public abstract void removeRoutingException(Serializable routingExceptionId);

    public Group getDefaultAutoAttendantGroup();

    public AutoAttendant newAutoAttendantWithDefaultGroup();
}
