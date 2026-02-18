@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
import org.qtproject.qt.qtjenny.HandyHelper
import org.qtproject.qt.qtjenny.MethodOverloadResolver.MethodRecord
import javax.lang.model.type.TypeKind
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jtefield_getterGenerated {
companion object {
	@JvmField val JTE_NAME = "field_getter.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,17,18,19,21,21,21,21,21,22,22,23,24,24,25,25,25,25,25,25,25,25,26,26,26,26,26,26,26,26,26,27,27,31,31,31,21,21,21,21,21)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		val classParam = if (jteData.rawStaticMod != "") "FULL_CLASS_NAME," else ""
		val thizParam = if (jteData.rawStaticMod != "") "QJniObject::" else "m_jniObject."
		jteOutput.writeContent("    ")
		jteOutput.writeUserContent(jteData.fieldComment)
		jteOutput.writeContent("\r\n    ")
		jteOutput.writeUserContent(jteData.rawStaticMod)
		jteOutput.writeContent("auto get")
		jteOutput.writeUserContent(jteData.fieldCamelCaseName)
		jteOutput.writeContent("(")
		jteOutput.writeUserContent(jteData.param)
		jteOutput.writeContent(") ")
		jteOutput.writeUserContent(jteData.constMod)
		jteOutput.writeContent("{\r\n       return ")
		jteOutput.writeUserContent(thizParam)
		jteOutput.writeContent("get")
		jteOutput.writeUserContent(jteData.static)
		jteOutput.writeContent("Field<")
		jteOutput.writeUserContent(jteData.jniReturnType)
		jteOutput.writeContent(">(")
		jteOutput.writeUserContent(classParam)
		jteOutput.writeContent("\"")
		jteOutput.writeUserContent(
                        jteData.field!!.simpleName.toString())
		jteOutput.writeContent("\");\r\n\r\n    }\r\n\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
