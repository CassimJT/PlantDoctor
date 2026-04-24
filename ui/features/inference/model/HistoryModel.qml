pragma Singleton
import QtQuick
import QtQuick.Controls
import QtCore

Item {
    id: historyModelManager

    property alias listmodel: listmodel

    // Signals for cloud synchronization
    signal syncToCloud(string diseaseName, double confidence, string location, string variaty)
    signal syncBatchToCloud(var dataArray)  // Array of objects with 4 fields
    signal syncCompleted(bool success, string message)

    // Persistent settings
    Settings {
        id: settings
    }

    // The actual model
    ListModel {
        id: listmodel
        // Define roles for better access
        property var roles: ["diseaseName", "classIndex", "date", "confidence", "location", "variaty"]
    }

    // --- Individual Getters ---

    function getDiseaseName(index) {
        if (index >= 0 && index < listmodel.count) {
            return listmodel.get(index).diseaseName
        }
        console.warn("Invalid index for getDiseaseName:", index)
        return ""
    }

    function getConfidence(index) {
        if (index >= 0 && index < listmodel.count) {
            return listmodel.get(index).confidence
        }
        console.warn("Invalid index for getConfidence:", index)
        return -1
    }

    function getLocation(index) {
        if (index >= 0 && index < listmodel.count) {
            return listmodel.get(index).location
        }
        console.warn("Invalid index for getLocation:", index)
        return ""
    }

    function getVariaty(index) {
        if (index >= 0 && index < listmodel.count) {
            return listmodel.get(index).variaty
        }
        console.warn("Invalid index for getVariaty:", index)
        return ""
    }

    // --- Array Getters for Batch Operations ---

    // Get all history entries as array of objects with 4 fields
    function getAllEntriesArray() {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            resultArray.push({
                diseaseName: entry.diseaseName,
                confidence: entry.confidence,
                location: entry.location,
                variaty: entry.variaty
            })
        }
        return resultArray
    }

    // Get unsynced entries as array
    function getUnsyncedEntriesArray() {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            if (!entry.synced) {
                resultArray.push({
                    diseaseName: entry.diseaseName,
                    confidence: entry.confidence,
                    location: entry.location,
                    variaty: entry.variaty
                })
            }
        }
        return resultArray
    }

    // Get entries by location as array
    function getEntriesByLocationArray(location) {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            if (entry.location === location) {
                resultArray.push({
                    diseaseName: entry.diseaseName,
                    confidence: entry.confidence,
                    location: entry.location,
                    variaty: entry.variaty
                })
            }
        }
        return resultArray
    }

    // Get entries by disease name as array
    function getEntriesByDiseaseArray(diseaseName) {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            if (entry.diseaseName === diseaseName) {
                resultArray.push({
                    diseaseName: entry.diseaseName,
                    confidence: entry.confidence,
                    location: entry.location,
                    variaty: entry.variaty
                })
            }
        }
        return resultArray
    }

    // Get latest N entries as array
    function getLatestEntriesArray(count) {
        var resultArray = []
        var startIndex = Math.max(0, listmodel.count - count)
        for (var i = startIndex; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            resultArray.push({
                diseaseName: entry.diseaseName,
                confidence: entry.confidence,
                location: entry.location,
                variaty: entry.variaty
            })
        }
        return resultArray
    }

    // Get entries within confidence range
    function getEntriesByConfidenceRange(minConfidence, maxConfidence) {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            if (entry.confidence >= minConfidence && entry.confidence <= maxConfidence) {
                resultArray.push({
                    diseaseName: entry.diseaseName,
                    confidence: entry.confidence,
                    location: entry.location,
                    variaty: entry.variaty
                })
            }
        }
        return resultArray
    }

    // Get entry as array of 4 fields (for single entry)
    function getEntryAsArray(index) {
        if (index >= 0 && index < listmodel.count) {
            var entry = listmodel.get(index)
            return [entry.diseaseName, entry.confidence, entry.location, entry.variaty]
        }
        return []
    }

    // Get all entries as 2D array
    function getAllEntriesAs2DArray() {
        var resultArray = []
        for (var i = 0; i < listmodel.count; i++) {
            var entry = listmodel.get(i)
            resultArray.push([entry.diseaseName, entry.confidence, entry.location, entry.variaty])
        }
        return resultArray
    }

    // Get all entries as JSON string
    function getAllEntriesAsJSON() {
        var dataArray = getAllEntriesArray()
        return JSON.stringify(dataArray)
    }

    // --- Batch Sync Functions ---

    // Sync all entries to cloud
    function syncAllToCloud() {
        var dataArray = getAllEntriesArray()
        if (dataArray.length > 0) {
            syncBatchToCloud(dataArray)
            console.log("Syncing", dataArray.length, "entries to cloud")
            return true
        }
        console.warn("No entries to sync")
        return false
    }

    // Sync unsynced entries only
    function syncUnsyncedToCloud() {
        var dataArray = getUnsyncedEntriesArray()
        if (dataArray.length > 0) {
            syncBatchToCloud(dataArray)
            console.log("Syncing", dataArray.length, "unsynced entries to cloud")
            return true
        }
        console.warn("No unsynced entries")
        return false
    }

    // Sync entries by location
    function syncByLocationToCloud(location) {
        var dataArray = getEntriesByLocationArray(location)
        if (dataArray.length > 0) {
            syncBatchToCloud(dataArray)
            console.log("Syncing", dataArray.length, "entries from location:", location)
            return true
        }
        return false
    }

    // Sync latest N entries
    function syncLatestToCloud(count) {
        var dataArray = getLatestEntriesArray(count)
        if (dataArray.length > 0) {
            syncBatchToCloud(dataArray)
            console.log("Syncing latest", dataArray.length, "entries")
            return true
        }
        return false
    }

    // --- Model management functions ---

    function addToHistory(diseaseName, classIndex, date, confidence, location, variaty) {
        // Validate required fields
        if (!diseaseName || diseaseName.trim() === "" || classIndex < 0 || !date || confidence === undefined || !location || location.trim() === "" || !variaty || variaty.trim() === "")
        {
            console.log("Invalid data: Cannot add empty field. Required: diseaseName, classIndex, date, confidence, location, variaty")
            return false
        }

        // Validate confidence range
        if (confidence < 0 || confidence > 1) {
            console.log("Invalid data: Confidence must be between 0 and 1")
            return false
        }

        listmodel.append({
            diseaseName: diseaseName,
            classIndex: classIndex,
            date: date,
            confidence: confidence,
            location: location,
            variaty: variaty,
            synced: false
        })
        console.log("Data added to history:", diseaseName, confidence, location, variaty)
        persistHistory()
        return true
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
            if (field) {
                data.push({
                    diseaseName: field.diseaseName,
                    classIndex: field.classIndex,
                    date: field.date,
                    confidence: field.confidence,
                    location: field.location,
                    variaty: field.variaty,
                    synced: field.synced || false
                })
            }
        }
        try {
            const jsonData = JSON.stringify(data)
            settings.setValue("historyModel", jsonData)
            console.log("History persisted with", listmodel.count, "items")
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
        parsedData.forEach(item => {
            listmodel.append({
                diseaseName: item.diseaseName,
                classIndex: item.classIndex,
                date: item.date,
                confidence: item.confidence,
                location: item.location || "Unknown",
                variaty: item.variaty || "Unknown",
                synced: item.synced || false
            })
        })
        console.log("History loaded with", listmodel.count, "items")
    }

    // Mark entries as synced
    function markAllAsSynced() {
        for (var i = 0; i < listmodel.count; i++) {
            if (!listmodel.get(i).synced) {
                listmodel.setProperty(i, "synced", true)
            }
        }
        persistHistory()
        console.log("All entries marked as synced")
    }

    function markEntriesAsSynced(indices) {
        for (var i = 0; i < indices.length; i++) {
            if (indices[i] >= 0 && indices[i] < listmodel.count) {
                listmodel.setProperty(indices[i], "synced", true)
            }
        }
        persistHistory()
        console.log("Marked", indices.length, "entries as synced")
    }
}