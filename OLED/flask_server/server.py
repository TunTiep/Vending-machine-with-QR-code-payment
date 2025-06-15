from flask import Flask, jsonify, request
from flask_socketio import SocketIO, emit
from collections import deque
app = Flask(__name__)

socketio = SocketIO(app)

payment_queue = deque(maxlen=10)

@app.route('/check-payment', methods=['GET'])
def check_payment():
    print(f"[CHECK-PAYMENT] payment_queue: {list(payment_queue)}")
    if payment_queue:
        product = payment_queue[0]
        if product in ["coca", "pepsi"]:  # Kiểm tra product hợp lệ
            print(f"[CHECK-PAYMENT] Trả về product: {product}")
            response = {"status": "success", "product": product}
            print(f"[CHECK-PAYMENT] Phản hồi: {response}")
            return jsonify(response)
        else:
            print("[CHECK-PAYMENT] Lỗi: product không hợp lệ")
            payment_queue.popleft()  # Xóa product không hợp lệ
            return jsonify({"status": "error", "message": "Invalid product"})
    print("[CHECK-PAYMENT] Hàng đợi rỗng, trả về pending")
    return jsonify({"status": "pending"})

@app.route('/confirm-payment', methods=['POST'])
def confirm_payment():
    print(f"[CONFIRM-PAYMENT] Xóa product khỏi payment_queue")
    if payment_queue:
        payment_queue.popleft()
    print(f"[CONFIRM-PAYMENT] payment_queue sau khi xóa: {list(payment_queue)}")
    return jsonify({"status": "confirmed"})

@app.route('/sepay-webhook', methods=['POST'])
def sepay_webhook():
    data = request.get_json()
    print(f"[WEBHOOK] Dữ liệu nhận được: {data}")
    try:
        amount = float(data.get("transferAmount", 0))
        description = str(data.get("description", "")).lower()
    except (TypeError, ValueError):
        print("[WEBHOOK] Lỗi: Dữ liệu transferAmount hoặc description không hợp lệ")
        return jsonify({"status": "invalid data"}), 400

    if any(kw in description for kw in ["muahang", "thanh toan"]):
        if amount == 3000:
            payment_queue.append("pepsi")
        elif amount == 2000:
            payment_queue.append("coca")
        else:
            print(f"[WEBHOOK] Lỗi: Số tiền không hợp lệ: {amount}")
            return jsonify({"status": "invalid amount"}), 400

        print(f"[WEBHOOK] payment_queue sau khi thêm: {list(payment_queue)}")
        socketio.emit('payment_status', {'status': 'success', 'product': payment_queue[-1]})
        return jsonify({"status": "success"})

    print("[WEBHOOK] Yêu cầu bị bỏ qua: Không chứa từ khóa hợp lệ")
    return jsonify({"status": "ignored"}), 400

if __name__ == '__main__':
    print("Khởi động server Flask...")
    socketio.run(app, host='0.0.0.0', port=5000)