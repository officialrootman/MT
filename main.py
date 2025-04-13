import pywhatkit
import time
from datetime import datetime

def send_whatsapp_message(phone_number, message):
    try:
        # Remove any spaces or special characters from phone number
        phone_number = phone_number.replace(" ", "").replace("-", "")
        
        # Get current time for sending message
        now = datetime.now()
        hour = now.hour
        minute = now.minute + 1  # Send message in the next minute
        
        # Send the message
        pywhatkit.sendwhatmsg(phone_number, message, hour, minute)
        print("Message sent successfully!")
        
    except Exception as e:
        print(f"An error occurred: {str(e)}")

# Example usage:
# Note: Phone number should be in international format with country code
# phone_number = "+90XXXXXXXXXX"  # Replace with the actual phone number
# message = "Merhaba! Bu bir test mesajıdır."
# send_whatsapp_message(phone_number, message)