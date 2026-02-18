@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
import org.qtproject.qt.qtjenny.HandyHelper
import org.qtproject.qt.qtjenny.MethodOverloadResolver.MethodRecord
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jtemethod_prologueGenerated {
companion object {
	@JvmField val JTE_NAME = "method_prologue.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,17,18,20,20,20,20,20,22,22,22,23,25,25,27,27,28,30,30,32,32,20,20,20,20,20)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("\r\n")
		if (jteData.useJniHelper) {
			if (jteData.isStatic) {
				jteOutput.writeContent("::jenny::Env env; assertInited(env.get());\r\n")
			} else {
				jteOutput.writeContent("::jenny::Env env; ::jenny::LocalRef<jobject> jennyLocalRef = getThis(false); jobject thiz = jennyLocalRef.get();\r\n")
			}
		} else {
			jteOutput.writeContent("assertInited(env);\r\n")
		}
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
