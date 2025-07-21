#define SERVER_CODE \
"\nconst express = require('express');" \
"\nconst cors = require('cors');" \
"\nconst app = express();" \
"\n" \
"\n// Habilita CORS para permitir requisições do frontend" \
"\napp.use(cors());" \
"\napp.use(express.json());" \
"\n" \
"\n// Configurações padrão" \
"\nlet config = {" \
"\n    temperature: { min: 10, max: 30 }," \
"\n    humidity: { min: 30, max: 70 }," \
"\n    seaLevelPressure: 101325.0" \
"\n};" \
"\n" \
"\n// Rota para obter configurações" \
"\napp.get('/api/config', (req, res) => {" \
"\n    res.json(config);" \
"\n});" \
"\n" \
"\n// Rota para atualizar configurações" \
"\napp.post('/api/config', (req, res) => {" \
"\n    config = { ...config, ...req.body };" \
"\n    res.json({ success: true, config });" \
"\n});" \
"\n" \
"\n// Simula a leitura de sensores reais" \
"\nfunction readSensors() {" \
"\n    // Valores simulados - substitua por leituras reais dos sensores" \
"\n    return {" \
"\n        aht: {" \
"\n            temp: (Math.random() * 10 + 20).toFixed(1), // 20-30°C" \
"\n            humidity: (Math.random() * 20 + 40).toFixed(1) // 40-60%" \
"\n        }," \
"\n        bmp: {" \
"\n            pressure: (Math.random() * 1000 + 95000).toFixed(1), // 95000-96000 Pa" \
"\n            temp: (Math.random() * 5 + 18).toFixed(1) // 18-23°C" \
"\n        }" \
"\n    };" \
"\n}" \
"\n" \
"\n// Rota para obter dados dos sensores" \
"\napp.get('/api/sensor-data', (req, res) => {" \
"\n    const sensorData = readSensors();" \
"\n    " \
"\n    // Calcula a média de temperatura" \
"\n    sensorData.average = {" \
"\n        temp: ((parseFloat(sensorData.aht.temp) + parseFloat(sensorData.bmp.temp)) / 2).toFixed(1)" \
"\n    };" \
"\n    " \
"\n    res.json(sensorData);" \
"\n});" \
"\n" \
"\n// Inicia o servidor" \
"\nconst PORT = process.env.PORT || 3000;" \
"\napp.listen(PORT, () => {" \
"\n    console.log(`Servidor rodando na porta ${PORT}`);" \
"\n});"