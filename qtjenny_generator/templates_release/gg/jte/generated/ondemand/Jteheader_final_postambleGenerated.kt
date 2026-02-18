@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jteheader_final_postambleGenerated {
companion object {
	@JvmField val JTE_NAME = "header_final_postamble.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,18,18,18,18,18,20,20,20,20,20,20,20,20,22,22,22,18,18,18,18,18)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("\r\n")
		if (jteData.environment.configurations.headerOnlyProxy) {
			jteOutput.writeContent("#endif // ")
			jteOutput.writeUserContent(jteData.namespaceHelper.fileNamePrefix)
			jteOutput.writeUserContent(jteData.className)
			jteOutput.writeContent("_H")
		}
		jteOutput.writeContent("\r\n\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
