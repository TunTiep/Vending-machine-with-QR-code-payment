Flowchart (Algorithm Flow Diagram)

![image](https://github.com/user-attachments/assets/1e9bd17f-759f-45d1-b53e-4f08ccc420dc)

1. User presses the product selection button (Coca/Pepsi)
- The user selects a product using a physical button.
- Each button corresponds to a specific drink (Coca or Pepsi).
2. ESP32 processes the selection and sends a request
- The ESP32 detects which product the user selected.
- Based on the selection, the ESP32 sends an HTTP request to the Flask server to notify which product is being purchased.
3. Simultaneously, the OLED screen displays the interface / Flask server confirms the product
- The Flask server receives the request from the ESP32 and records the selected product.
- The server then prepares to wait for the payment for that product.
4. The Flask server calls the Sepay service to monitor bank transactions
- Flask either periodically checks or uses a webhook to track incoming transfers.
- If it detects a transaction with the correct amount (e.g., 2000 VND with the note 'muahang coca' or 3000 VND with 'muahang pepsi'), the transaction is marked as valid.
5. Condition: Was the payment successful?
If the payment is successful:
- The Flask server sends a confirmation back to the ESP32 indicating that the money was received.
- The ESP32 activates a servo to dispense the product to the user.
If the payment was not successful or the information was incorrect:
- The server notifies the ESP32 of the failure.
- The ESP32 displays an error message on the OLED, such as: "Payment failed, please try again."

System Block Diagram

![image](https://github.com/user-attachments/assets/2c4c7181-7636-4b29-9e24-2ed59191de70)
