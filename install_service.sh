#!/bin/bash
set -e  # Exit immediately if any command fails

# Check for username argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <linux_username>"
    exit 1
fi

USER_NAME="$1"

# Use the current directory as working directory
WORKDIR="$(pwd)"
EXEC="$WORKDIR/build/Release/binance-trading"
OUTPUT_DIR="$WORKDIR/output"
LOG="$WORKDIR/binance-trading.log"
ERR="$WORKDIR/binance-trading.err"
STATISTICS_FILE="$OUTPUT_DIR/statistics.txt"

SERVICE_NAME="binance-trading"
SERVICE_FILE="/etc/systemd/system/$SERVICE_NAME.service"

# Check if the executable exists
if [ ! -f "$EXEC" ]; then
    echo "Error: $EXEC does not exist. Please build the project first."
    exit 1
fi

# Create output folder if not exists
mkdir -p "$OUTPUT_DIR"
chown -R "$USER_NAME":"$USER_NAME" "$OUTPUT_DIR"

# Create the systemd service file
echo "Creating $SERVICE_FILE ..."
sudo bash -c "cat > $SERVICE_FILE" <<EOL
[Unit]
Description=Binance Trading
After=network.target

[Service]
Type=simple
User=$USER_NAME
WorkingDirectory=$WORKDIR
Environment="OUTPUT_FILE=$STATISTICS_FILE"
ExecStart=$EXEC
Restart=always
RestartSec=10
StandardOutput=append:$LOG
StandardError=append:$ERR

[Install]
WantedBy=multi-user.target
EOL

# Reload systemd and enable service
echo "Reloading systemd ..."
sudo systemctl daemon-reload
sudo systemctl enable $SERVICE_NAME
sudo systemctl restart $SERVICE_NAME

echo "Service $SERVICE_NAME installed and started for user $USER_NAME."
echo "Statistics output: $STATISTICS_FILE"

# Check service status 
sudo systemctl status binance-trading
