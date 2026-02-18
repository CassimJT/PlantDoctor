@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
import org.qtproject.qt.qtjenny.HandyHelper
import org.qtproject.qt.qtjenny.MethodOverloadResolver.MethodRecord
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jteconstructor_definitionGenerated {
companion object {
	@JvmField val JTE_NAME = "constructor_definition.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,17,18,20,20,20,20,20,22,22,22,22,22,22,22,23,23,23,23,23,23,24,24,25,25,25,25,28,28,28,20,20,20,20,20)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("\r\n    // construct: ")
		jteOutput.writeUserContent(jteData.handyHelper.getModifiers(jteData.method!!.method))
		jteOutput.writeContent(" ")
		jteOutput.writeUserContent(jteData.simpleClassName)
		jteOutput.writeContent("(")
		jteOutput.writeUserContent(jteData.handyHelper.getJavaMethodParam(jteData.method!!.method))
		jteOutput.writeContent(")\r\n    static ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent(" newInstance")
		jteOutput.writeUserContent(jteData.method!!.resolvedPostFix)
		jteOutput.writeContent("(")
		jteOutput.writeUserContent(jteData.param)
		jteOutput.writeContent(") {\r\n        ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent(" ret;\r\n        ret.m_jniObject = QJniObject(FULL_CLASS_NAME, \"")
		jteOutput.writeUserContent(jteData.handyHelper.getBinaryMethodSignature(jteData.method!!.method))
		jteOutput.writeContent("\"")
		jteOutput.writeUserContent(jteData.handyHelper.getJniMethodParamVal(jteData.clazz!!, jteData.method!!.method, jteData.useJniHelper))
		jteOutput.writeContent(");\r\n        return ret;\r\n    }\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
