package de.dhbw.pdv.ex;

import org.eclipse.osgi.framework.console.CommandProvider;

public class CliEx implements CommandProvider {

	@Override
	public String getHelp() {
		StringBuffer buf = new StringBuffer();
		buf.append("Geben sie einen der folgenden Befehl ein.");
		return buf.toString();
	}

}
