
# 💧 Projeto IoT – Monitoramento de Água com ESP32 e MQTT

Este projeto tem como objetivo monitorar a qualidade e o nível da água de um reservatório utilizando sensores conectados ao ESP32, com envio de dados para a internet via protocolo MQTT.

---

## 🔧 Componentes Utilizados

| Componente              | Modelo           | Função no Projeto                                    |
|-------------------------|------------------|------------------------------------------------------|
| Microcontrolador        | ESP32 DevKit V1  | Controlador principal com Wi-Fi                     |
| Sensor de Nível         | HC-SR04          | Mede a distância até o nível da água no reservatório |
| Sensor de Turbidez      | SEN0189/SEN0193  | Mede a qualidade (transparência) da água            |
| Módulo Relé 1 canal     | JQC-3FF-S-Z      | Aciona a válvula solenoide                          |
| Atuador                 | Válvula IHLT10747| Libera ou bloqueia o fluxo de água (12V)            |
| Protoboard e jumpers    | -                | Montagem dos componentes                            |
| Fonte 12V 2A P4         | -                | Alimentação da válvula via relé                     |

---

## 🔌 Esquemas e Diagramas

Diagramas disponíveis no repositório:

- 📘 `esquema.png`: Esquema eletrônico com ligações
- 🔁 `Fluxograma.png`: Fluxo lógico de funcionamento

---

## 🌐 Integração com o MQTT

- **Broker MQTT:** `broker.hivemq.com`
- **Porta:** `1883`
- **Bibliotecas:** `WiFi.h`, `PubSubClient.h`

### 📤 Tópicos Publicados

| Tópico                  | Descrição                                       |
|-------------------------|--------------------------------------------------|
| `iotmack/turbidez`      | Valor da turbidez da água (0–1023)              |
| `iotmack/nivel`         | Distância do sensor HC-SR04 à água (em cm)      |
| `iotmack/valvula`       | Estado da válvula: `ABERTA` ou `FECHADA`        |
| `iotmack/status_wifi`   | Conectado ou desconectado do Wi-Fi              |
| `iotmack/status_mqtt`   | Conectado ou desconectado do broker MQTT        |

- Os dados são enviados **a cada 3 segundos**.
- Conexão MQTT tenta reconectar 2 vezes, e aguarda **5 minutos** em caso de falha.

---

## ✅ Lógica de Segurança

- A válvula **fecha** se:
  - A turbidez estiver **abaixo de 900**
  - OU o nível da água ultrapassar **15 cm** (reservatório tem 20 cm)
- Caso contrário, a válvula permanece **aberta**

---

## 📽️ Demonstração em Vídeo

🔗 (https://youtu.be/-qdWFvEU6Dw?si=GXl9Yc0zR4AFu6qM)

---

---

## 🧠 Propostas Futuras

- Sensores adicionais (PH, condutividade)
- Algoritmos preditivos para alertas
- Integração com SMS/app via notificações
- Controle remoto por dashboard web ou app

---
