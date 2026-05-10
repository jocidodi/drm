#!/bin/bash

# 1. Check for sudo/root permissions
if [ "$EUID" -ne 0 ]; then
  echo "❌ Error: This script requires sudo permissions."
  echo "Run it as: sudo ./drop_and_copy.sh"
  exit 1
fi

echo "===================================================="
echo "⚠️  WARNING: This will copy folders to the ROOT / directory!"
echo "⚠️  WARNING: It will also DELETE a Kernel Collection file!"
echo "===================================================="
echo ""
echo "Drag and drop your folders here, one by one."
echo "Press [ENTER] after each drop."
echo "Type 'done' when you have dropped all your folders."
echo ""

FOLDERS=()

# 2. Wait for user to drag and drop folders
while true; do
    # -r prevents backslashes from acting as escape characters (important for drag-and-drop paths)
    read -rp "Drop folder (or type 'done'): " dropped_path
    
    # Clean up the path (remove trailing spaces or accidental quotes from dragging)
    dropped_path=$(echo "$dropped_path" | sed 's/^ *//;s/ *$//' | sed 's/^"//;s/"$//' | sed "s/^'//;s/'$//")

    if [[ "$dropped_path" == "done" ]]; then
        break
    fi

    # Extract the folder name from the path
    folder_name=$(basename "$dropped_path")

    # 🚨 SAFETY CHECK: Abort if the folder is named "System"
    if [[ "$folder_name" == "System" ]]; then
        echo ""
        echo "🛑 ABORTING: Folder named 'System' detected!"
        echo "Copying a System folder to / will break macOS."
        exit 1
    fi
    
    # 🚨 EXTRA SAFETY: Abort for other critical macOS folders
    if [[ "$folder_name" == "usr" || "$folder_name" == "bin" || "$folder_name" == "sbin" || "$folder_name" == "var" ]]; then
        echo ""
        echo "🛑 ABORTING: Folder named '$folder_name' detected!"
        echo "Copying this to / will break macOS."
        exit 1
    fi

    # Check if the dropped item actually exists and is a directory
    if [[ -d "$dropped_path" ]]; then
        FOLDERS+=("$dropped_path")
        echo "✅ Added: $dropped_path"
    else
        echo "❌ Invalid folder. Please drop a valid directory."
    fi
done

# Check if any folders were added
if [ ${#FOLDERS[@]} -eq 0 ]; then
  echo "No folders were provided. Exiting."
  exit 0
fi

echo ""
echo "Starting copy and attribute changes..."
echo ""

# 3. Process each folder
for folder in "${FOLDERS[@]}"; do
    # Get just the folder name again for the destination path
    folder_name=$(basename "$folder")
    dest="/$folder_name"

    echo "--------------------------------------------------"
    echo "Processing: $folder -> $dest"

    # Copy using ditto (macOS native command, handles subfolders and file attributes perfectly)
    echo "⏳ Copying files..."
    ditto "$folder" "$dest"

    # Change permissions to 755 recursively
    echo "⏳ Applying chmod -R 755..."
    chmod -R 755 "$dest"

    # Change ownership to root:wheel recursively
    echo "⏳ Applying chown -R root:wheel..."
    chown -R root:wheel "$dest"

    echo "✅ Finished processing $folder_name"
done

# 4. Delete the specific Kernel Collection file
TARGET_KC="/Library/KernelCollections/AuxiliaryKernelExtensions.kc"
echo "--------------------------------------------------"
echo "🗑️ Attempting to remove $TARGET_KC..."

if [ -f "$TARGET_KC" ]; then
    rm -f "$TARGET_KC"
    if [ $? -eq 0 ]; then
        echo "✅ Successfully removed $TARGET_KC"
    else
        echo "❌ Failed to remove $TARGET_KC (System Integrity Protection may be blocking this)"
    fi
else
    echo "⚠️ File $TARGET_KC not found. Skipping removal."
fi

# 5. Trigger macOS permissions/authorization prompt for Kernel Extensions
echo "--------------------------------------------------"
echo "🔔 Touching /Library/Extensions to trigger macOS kext authorization..."
if [ -d "/Library/Extensions" ]; then
    touch /Library/Extensions
    echo "✅ Done. macOS should now prompt for kernel extension authorization (if applicable)."
else
    echo "⚠️ /Library/Extensions does not exist. Skipping touch."
fi

echo "--------------------------------------------------"
echo "🎉 All operations completed!"