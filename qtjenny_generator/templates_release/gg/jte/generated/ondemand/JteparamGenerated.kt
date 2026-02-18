@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class JteparamGenerated {
companion object {
	@JvmField val JTE_NAME = "param.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,18,18,18,18,18,20,20,20,21,22,22,23,24,24,26,26,18,18,18,18,18)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("\r\n")
		if (!jteData.useJniHelper) {
			jteOutput.writeUserContent(jteData.param)
			jteOutput.writeContent("\r\n")
		} else {
			jteOutput.writeUserContent(jteData.param)
			jteOutput.writeContent("\r\n")
		}
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
