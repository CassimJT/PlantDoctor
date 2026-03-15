import torch
from pathlib import Path
from ultralytics import YOLO

# ------------------------------
# 1. Paths
# ------------------------------
model_weights_path = Path("/home/csociety/Downloads/best(1).pt")  # downloaded Hugging Face model
output_torchscript_path = Path("/home/csociety/Downloads/plant_leaf_model.ts.pt")  # output TorchScript

# ------------------------------
# 2. Load YOLOv8 model
# ------------------------------
model = YOLO(str(model_weights_path))  # automatically loads YOLOv8 model
model.model.eval()  # set eval mode

# ------------------------------
# 3. Dummy input
# ------------------------------
# adjust size if needed; YOLOv8 default is 640x640
dummy_input = torch.randn(1, 3, 640, 640)

# ------------------------------
# 4. Convert to TorchScript
# ------------------------------
scripted_model = torch.jit.trace(model.model, dummy_input)

# ------------------------------
# 5. Save TorchScript model
# ------------------------------
scripted_model.save(str(output_torchscript_path))
print(f"TorchScript model saved at: {output_torchscript_path}")
