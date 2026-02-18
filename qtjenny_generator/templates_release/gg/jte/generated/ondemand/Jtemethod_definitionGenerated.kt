@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
import org.qtproject.qt.qtjenny.HandyHelper
import org.qtproject.qt.qtjenny.MethodOverloadResolver.MethodRecord
import javax.lang.model.type.TypeKind
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jtemethod_definitionGenerated {
companion object {
	@JvmField val JTE_NAME = "method_definition.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,17,18,19,21,21,21,21,21,22,22,23,25,25,25,25,25,25,25,29,29,29,29,29,30,30,30,30,30,30,30,30,30,31,31,31,31,31,31,31,31,31,32,32,33,33,33,33,37,37,37,21,21,21,21,21)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		val classParam = if (jteData.rawStaticMod != "") "FULL_CLASS_NAME," else ""
		val thizParam = if (jteData.rawStaticMod != "") "QJniObject::" else "m_jniObject."
		jteOutput.writeContent("    // method: ")
		jteOutput.writeUserContent(jteData.handyHelper.getModifiers(jteData.method!!.method))
		jteOutput.writeContent(" ")
		jteOutput.writeUserContent(jteData.method!!.method.returnType.toString())
		jteOutput.writeContent(" ")
		jteOutput.writeUserContent(jteData.method!!.method.simpleName.toString())
		jteOutput.writeContent("(")
		jteOutput.writeUserContent(
                        jteData.handyHelper.getJavaMethodParam(
                            jteData.method!!.method
                        )
                    )
		jteOutput.writeContent(")\r\n    ")
		jteOutput.writeUserContent(jteData.rawStaticMod)
		jteOutput.writeContent("auto ")
		jteOutput.writeUserContent(jteData.method!!.method.simpleName.toString())
		jteOutput.writeUserContent(jteData.method!!.resolvedPostFix)
		jteOutput.writeContent("(")
		jteOutput.writeUserContent(jteData.param)
		jteOutput.writeContent(") ")
		jteOutput.writeUserContent(jteData.rawConstMod)
		jteOutput.writeContent("{\r\n        ")
		jteOutput.writeUserContent(jteData.returnStatement)
		jteOutput.writeUserContent(thizParam)
		jteOutput.writeContent("call")
		jteOutput.writeUserContent(jteData.static)
		jteOutput.writeContent("Method<")
		jteOutput.writeUserContent(jteData.jniReturnType)
		jteOutput.writeContent(">(")
		jteOutput.writeUserContent(classParam)
		jteOutput.writeContent("\r\n                        \"")
		jteOutput.writeUserContent(jteData.method!!.method.simpleName.toString())
		jteOutput.writeContent("\",\r\n                        \"")
		jteOutput.writeUserContent(jteData.handyHelper.getBinaryMethodSignature(jteData.method!!.method))
		jteOutput.writeContent("\"")
		jteOutput.writeUserContent(jteData.handyHelper.getJniMethodParamVal(jteData.clazz!!, jteData.method!!.method!!, jteData.useJniHelper))
		jteOutput.writeContent(");\r\n    }\r\n\r\n\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
