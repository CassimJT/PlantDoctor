//getting current time
function getCurrentTime() {
    var currentData = new Date();
    var hours = currentData.getHours();
    var minites = currentData.getMinutes();
    var ampm = hours >=12 ?"PM" : "AM"

    var formrmatedHour = hours < 10 ? "0" + hours : hours
    var formatedMinites = minites < 10 ? "0" + minites : minites

    return formrmatedHour + ":" + formatedMinites +" "+ ampm
}

//get curretDate
function getCurrentDate() {
    const currentDate = new Date();
    const year = currentDate.getFullYear();
    const month = String(currentDate.getMonth() + 1).padStart(2, "0"); // Months are 0-based
    const day = String(currentDate.getDate()).padStart(2, "0");
    return `${year}-${month}-${day}`; // Return the formatted date
}

//togle bacnk and front camera
function backFrontCameraClicked(mediaDevices) {
    var listOfCameras = mediaDevices.videoInputs
    if (camera.cameraDevice.position === CameraDevice.FrontFace) {
        for (var i = 0; i < listOfCameras.length; i++) {
            if (listOfCameras[i].position === CameraDevice.BackFace) {
                camera.cameraDevice = listOfCameras[i]
                return
            }
        }
    } else {
        for (var j = 0; j < listOfCameras.length; j++) {
            if (listOfCameras[j].position === CameraDevice.FrontFace) {
                camera.cameraDevice = listOfCameras[j]
                return
            }
        }
    }
}

// --------- upload infarence ------------------------
function uploadForInfarance() {
    var preview = Helper.imagePreview()
    var path = Helper.localFilePath()
    let file = preview || path
    InfarenceRunner.classifyImage(file)
}
