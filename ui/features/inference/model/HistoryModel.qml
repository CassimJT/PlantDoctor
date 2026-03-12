pragma Singleton
import QtQuick
import QtQuick.Controls
import QtCore

Item {
    id: historyModelManager

    property alias listmodel: listmodel
    // Persistent settings
    Settings {
        id: settings
    }

    // The actual model
    ListModel {
        id: listmodel

    }

    // --- Model management functions ---

    function addToHistory(diseaseName, classIndex, date, confidence) {
        if (!diseaseName || diseaseName.trim() === "" || classIndex < 0 || !date || confidence === undefined)
        {
            console.log("Invalid data: Cannot add empty field.")
            return
        }
        listmodel.append({ diseaseName: diseaseName, classIndex: classIndex, date: date, confidence:confidence })
        console.log("Data added to history:", diseaseName, classIndex, date,confidence)
    }

    function clearModel() {
        listmodel.clear()
        persistHistory()
    }

    function modelSize() {
        return listmodel.count
    }

    function deleteHistory(index) {
        if (index >= 0 && index < listmodel.count)
            listmodel.remove(index)
        persistHistory()
    }

    function persistHistory() {
        const data = []

        for (let i = 0; i < listmodel.count; i++) {
            const field = listmodel.get(i)
            if (field) data.push(field)
        }

        try {
            const jsonData = JSON.stringify(data)
            settings.setValue("historyModel", jsonData)
            console.log("History persisted.")
        } catch (error) {
            console.log(`Error stringifying data: ${error}`)
        }
    }

    function loadHistory() {
        const savedData = settings.value("historyModel", "[]")
        let parsedData

        try {
            parsedData = JSON.parse(savedData)

        } catch (error) {
            console.log(`Error parsing data: ${error}`)
            return
        }

        if (!Array.isArray(parsedData) || parsedData.length === 0) {
            console.log("No saved data found.")
            return
        }

        clearModel()
        parsedData.forEach(item => listmodel.append(item))
        console.log("History loaded.")
    }

}
