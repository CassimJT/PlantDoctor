@file:Suppress("ktlint")
package gg.jte.generated.ondemand
import org.qtproject.qt.qtjenny.JteData
import org.qtproject.qt.qtjenny.Constants
@Suppress("UNCHECKED_CAST", "UNUSED_PARAMETER")
class Jteheader_preambleGenerated {
companion object {
	@JvmField val JTE_NAME = "header_preamble.kte"
	@JvmField val JTE_LINE_INFO = intArrayOf(15,15,15,16,18,18,18,18,18,20,20,20,21,21,21,22,22,22,27,27,28,28,33,33,37,37,38,38,41,41,44,44,45,45,50,50,50,18,18,18,18,18)
	@JvmStatic fun render(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, jteData:JteData) {
		jteOutput.writeContent("\r\n")
		jteOutput.writeUserContent(Constants.AUTO_GENERATE_NOTICE)
		jteOutput.writeContent("\r\n#ifndef ")
		jteOutput.writeUserContent(jteData.namespaceHelper.fileNamePrefix)
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent("_H\r\n#define ")
		jteOutput.writeUserContent(jteData.namespaceHelper.fileNamePrefix)
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent("_H\r\n\r\n#include <QJniObject>\r\n#include <cmath>\r\n\r\n")
		jteOutput.writeUserContent(jteData.namespaceHelper.beginNamespace())
		jteOutput.writeContent("\r\nclass ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent(" {\r\nprivate:\r\n    QJniObject m_jniObject;\r\n    static constexpr double NaN = NAN;\r\npublic:\r\n    static constexpr auto FULL_CLASS_NAME = \"")
		jteOutput.writeUserContent(jteData.slashClassName)
		jteOutput.writeContent("\";\r\n    QJniObject getJniObject() const {return m_jniObject;}\r\n    const QJniObject* operator->() const {return &m_jniObject;}\r\n    template <class T> operator T() {return m_jniObject.object<T>();}\r\n    ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent("() {}\r\n    ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent("(const QJniObject& jniObject) {\r\n        m_jniObject = jniObject;\r\n    }\r\n    ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent("(jobject globalRef) {\r\n        m_jniObject = QJniObject(globalRef);\r\n    }\r\n    static ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent(" fromLocalRef(jobject localRef) {\r\n        ")
		jteOutput.writeUserContent(jteData.className)
		jteOutput.writeContent(" res;\r\n    res.m_jniObject = QJniObject::fromLocalRef(localRef);\r\n    return res;\r\n    }\r\n\r\n")
	}
	@JvmStatic fun renderMap(jteOutput:gg.jte.TemplateOutput, jteHtmlInterceptor:gg.jte.html.HtmlInterceptor?, params:Map<String, Any?>) {
		val jteData = params["jteData"] as JteData
		render(jteOutput, jteHtmlInterceptor, jteData);
	}
}
}
