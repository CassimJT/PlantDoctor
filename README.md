# PlantDoctor Android App

PlantDoctor is an Android mobile application designed to assist farmers in identifying crop diseases using AI-powered image analysis.

The application is built for offline-first environments to support farmers in rural areas with limited internet connectivity.

---

# Features

* AI-based crop disease diagnosis
* Offline-first mobile inference
* Camera-based leaf image capture
* Disease treatment recommendations
* Farmer-friendly mobile interface
* Local data storage and synchronization
* Support for intermittent internet connectivity

---

# Technology Stack

| Layer            | Technology                             |
| ---------------- | -------------------------------------- |
| Mobile Framework | Qt                                     |
| Backend API      | Node.js                                |
| AI/ML            | PyTorch, ONNX, HuggingFace, Executorch |

---


# Requirements

## Android

* Android 8.0+
* Camera permission
* Storage permission
* Internet permission (optional)

## Development

* Qt 6+
* Qt Creator
* Android SDK
* Android NDK
* Python 3.10+
* Node.js

---

# Setup

## 1. Clone Repository

```bash id="d7r2r2"
git clone <repository-url>
cd PlantDoctor
```

---

## 2. Install Dependencies

### Python Dependencies

```bash id="9f5w8x"
pip install -r requirements.txt
```

### Node Backend

```bash id="c8e3pf"
cd backend
npm install
```

---

# Running the Android App

## Using Qt Creator

1. Open the project in Qt Creator
2. Configure Android Kit
3. Connect Android device or emulator
4. Build and Run

---

# AI Model Integration

The application supports:

* ONNX models
* PyTorch exported models
* Mobile AI inference
* Offline inference execution

Place AI models inside:

```text id="gk4d8p"
/models
```

---

# Backend Configuration

Create a `.env` file:

```env id="3nh0iu"
API_BASE_URL=http://your-server-ip:3000
WHATSAPP_TOKEN=your_token
WHATSAPP_PHONE_NUMBER_ID=your_phone_number_id
```

---

# Android Permissions

```xml id="w2yr6n"
<uses-permission android:name="android.permission.CAMERA"/>
<uses-permission android:name="android.permission.INTERNET"/>
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
```

---

# Main Objectives

The application aims to:

* Provide real-time crop disease diagnosis
* Support offline disease detection
* Reduce crop losses
* Improve access to agricultural diagnosis tools

---

# Future Improvements

* Multi-language support
* Cloud synchronization
* Farmer analytics dashboard
* Improved AI model accuracy
* Expanded disease dataset support

---

# Contributors

* PlantDoctor Development Team
* Researchers
* Agricultural Extension Services

---

# License

This project is part of the PlantDoctor research initiative.

