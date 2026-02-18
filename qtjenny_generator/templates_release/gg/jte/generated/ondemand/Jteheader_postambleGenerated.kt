@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jteheader_postambleGenerated {
companion object {
	@JvmField val JTE_NAME = "header_postamble.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,18,18,18,18,18,20,20,20,21,21,21,18,18,18,18,18)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("};\r\n")
		jteOutput.writeUserContent(jteData.namespaceHelper.endNamespace())
		jteOutput.writeContent("\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
