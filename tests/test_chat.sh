SERVER_PORT=8080
SERVER_BIN=./server
CLIENT_BIN=./client
LOG_DIR=logs
SERVER_LOG=$LOG_DIR/server_test.log

mkdir -p $LOG_DIR

echo "[TEST] Iniciando servidor na porta $SERVER_PORT..."
$SERVER_BIN $SERVER_PORT > $SERVER_LOG 2>&1 &
SERVER_PID=$!
sleep 1

echo "[TEST] Servidor rodando com PID $SERVER_PID"

for i in 1 2 3; do
    echo "[TEST] Iniciando cliente $i..."
    $CLIENT_BIN 127.0.0.1 $SERVER_PORT > $LOG_DIR/client${i}.log 2>&1 <<EOF &
msg-${i}-hello
msg-${i}-test
EOF
done

sleep 3

echo "[TEST] Encerrando servidor..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "[TEST] Resultados:"
ls -lh $LOG_DIR/client*.log

echo "=== Conteúdo do log do cliente 1 ==="
cat $LOG_DIR/client1.log

echo "=== Conteúdo do log do servidor ==="
cat $SERVER_LOG
