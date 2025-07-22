#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <mosquitto.h>

#define BROKER_HOST "localhost"
#define BROKER_PORT 1883
#define KEEP_ALIVE 60
#define MAX_MESSAGE_LEN 1024
#define MAX_TOPIC_LEN 256
#define MAX_USERNAME_LEN 64

// Struttura per rappresentare i diversi ruoli
typedef enum {
    ROLE_RETTORE,
    ROLE_SEGRETERIA,
    ROLE_DOCENTE,
    ROLE_STUDENTE
} user_role_t;

// Struttura per il client chat
typedef struct {
    struct mosquitto *mosq;
    user_role_t role;
    char username[MAX_USERNAME_LEN];
    char subscribe_topic[MAX_TOPIC_LEN];
    char publish_base_topic[MAX_TOPIC_LEN];
    int is_connected;
} mqtt_chat_client_t;

// Callback per la connessione
void on_connect(struct mosquitto *mosq, void *userdata, int result) {
    mqtt_chat_client_t *client = (mqtt_chat_client_t *)userdata;
    
    if (result == 0) {
        printf("✅ Connesso al broker MQTT come %s\n", client->username);
        client->is_connected = 1;
        
        // Subscribe al topic appropriato
        mosquitto_subscribe(mosq, NULL, client->subscribe_topic, 0);
        printf("📡 Sottoscritto al topic: %s\n", client->subscribe_topic);
    } else {
        printf("❌ Errore di connessione: %d\n", result);
    }
}

// Callback per la disconnessione
void on_disconnect(struct mosquitto *mosq, void *userdata, int result) {
    mqtt_chat_client_t *client = (mqtt_chat_client_t *)userdata;
    client->is_connected = 0;
    printf("🔌 Disconnesso dal broker\n");
}

// Callback per i messaggi ricevuti
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    mqtt_chat_client_t *client = (mqtt_chat_client_t *)userdata;
    
    if (message->payloadlen) {
        // Ottieni timestamp
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);
        
        // Stampa il messaggio ricevuto
        printf("\n💬 [%s] Topic: %s\n", timestamp, message->topic);
        printf("   Messaggio: %s\n", (char *)message->payload);
        printf(">> ");
        fflush(stdout);
    }
}

// Funzione per inizializzare il client in base al ruolo
void init_client_by_role(mqtt_chat_client_t *client, user_role_t role, const char *username) {
    client->role = role;
    strncpy(client->username, username, MAX_USERNAME_LEN - 1);
    client->is_connected = 0;
    
    switch (role) {
        case ROLE_RETTORE:
            strcpy(client->subscribe_topic, "universita/#");
            strcpy(client->publish_base_topic, "universita/amministrazione/rettore");
            break;
            
        case ROLE_SEGRETERIA:
            strcpy(client->subscribe_topic, "universita/personale/#");
            strcpy(client->publish_base_topic, "universita/personale/segreteria");
            break;
            
        case ROLE_DOCENTE:
            strcpy(client->subscribe_topic, "universita/personale/#");
            strcpy(client->publish_base_topic, "universita/personale/docenti");
            break;
            
        case ROLE_STUDENTE:
            strcpy(client->subscribe_topic, "universita/personale/studenti/#");
            strcpy(client->publish_base_topic, "universita/personale/studenti");
            break;
    }
}

// Funzione per pubblicare un messaggio
void publish_message(mqtt_chat_client_t *client, const char *message) {
    if (!client->is_connected) {
        printf("❌ Non connesso al broker!\n");
        return;
    }
    
    char full_topic[MAX_TOPIC_LEN];
    char full_message[MAX_MESSAGE_LEN];
    
    // Crea il topic completo con l'username
    snprintf(full_topic, sizeof(full_topic), "%s/%s", 
             client->publish_base_topic, client->username);
    
    // Crea il messaggio completo con nome utente
    snprintf(full_message, sizeof(full_message), "%s: %s", 
             client->username, message);
    
    int result = mosquitto_publish(client->mosq, NULL, full_topic, 
                                  strlen(full_message), full_message, 0, false);
    
    if (result == MOSQ_ERR_SUCCESS) {
        printf("📤 Messaggio inviato!\n");
    } else {
        printf("❌ Errore invio messaggio: %d\n", result);
    }
}

// Funzione per mostrare i comandi disponibili
void show_help() {
    printf("\n=== COMANDI DISPONIBILI ===\n");
    printf("/help    - Mostra questo aiuto\n");
    printf("/quit    - Esce dalla chat\n");
    printf("/status  - Mostra stato connessione\n");
    printf("/topic   - Mostra topic di sottoscrizione\n");
    printf("Qualsiasi altro testo verrà inviato come messaggio\n");
    printf("==========================\n\n");
}

// Funzione per scegliere il ruolo
user_role_t choose_role() {
    int choice;
    
    printf("\n=== SCEGLI IL TUO RUOLO ===\n");
    printf("1. Rettore (legge tutto)\n");
    printf("2. Segreteria (legge personale)\n");
    printf("3. Docente (legge personale)\n");
    printf("4. Studente (legge solo studenti)\n");
    printf("Scelta [1-4]: ");
    
    scanf("%d", &choice);
    getchar(); // Consuma il newline
    
    switch (choice) {
        case 1: return ROLE_RETTORE;
        case 2: return ROLE_SEGRETERIA;
        case 3: return ROLE_DOCENTE;
        case 4: return ROLE_STUDENTE;
        default: 
            printf("Scelta non valida, default: Studente\n");
            return ROLE_STUDENTE;
    }
}

int main() {
    mqtt_chat_client_t client;
    char username[MAX_USERNAME_LEN];
    char input[MAX_MESSAGE_LEN];
    int result;
    
    printf("🎓 === CHAT UNIVERSITARIA MQTT ===\n");
    
    // Inizializza la libreria mosquitto
    mosquitto_lib_init();
    
    // Scegli ruolo
    user_role_t role = choose_role();
    
    // Inserisci username
    printf("Inserisci il tuo username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi newline
    
    // Inizializza client
    init_client_by_role(&client, role, username);
    
    // Crea istanza mosquitto
    client.mosq = mosquitto_new(username, true, &client);
    if (!client.mosq) {
        printf("❌ Errore creazione client mosquitto\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    
    // Imposta callback
    mosquitto_connect_callback_set(client.mosq, on_connect);
    mosquitto_disconnect_callback_set(client.mosq, on_disconnect);
    mosquitto_message_callback_set(client.mosq, on_message);
    
    // Connetti al broker
    result = mosquitto_connect(client.mosq, BROKER_HOST, BROKER_PORT, KEEP_ALIVE);
    if (result != MOSQ_ERR_SUCCESS) {
        printf("❌ Errore connessione al broker: %d\n", result);
        mosquitto_destroy(client.mosq);
        mosquitto_lib_cleanup();
        return 1;
    }
    
    // Avvia il loop network in background
    mosquitto_loop_start(client.mosq);
    
    // Aspetta la connessione
    while (!client.is_connected) {
        usleep(100000); // 100ms
    }
    
    show_help();
    
    // Loop principale per input utente
    printf(">> ");
    while (fgets(input, sizeof(input), stdin)) {
        // Rimuovi newline
        input[strcspn(input, "\n")] = 0;
        
        // Gestisci comandi speciali
        if (strcmp(input, "/quit") == 0) {
            break;
        } else if (strcmp(input, "/help") == 0) {
            show_help();
        } else if (strcmp(input, "/status") == 0) {
            printf("Stato: %s\n", client.is_connected ? "Connesso" : "Disconnesso");
            printf("Ruolo: %s\n", 
                   role == ROLE_RETTORE ? "Rettore" :
                   role == ROLE_SEGRETERIA ? "Segreteria" :
                   role == ROLE_DOCENTE ? "Docente" : "Studente");
        } else if (strcmp(input, "/topic") == 0) {
            printf("Topic sottoscrizione: %s\n", client.subscribe_topic);
            printf("Topic pubblicazione: %s/%s\n", 
                   client.publish_base_topic, client.username);
        } else if (strlen(input) > 0) {
            publish_message(&client, input);
        }
        
        printf(">> ");
    }
    
    // Cleanup
    printf("\n👋 Disconnessione in corso...\n");
    mosquitto_loop_stop(client.mosq, false);
    mosquitto_disconnect(client.mosq);
    mosquitto_destroy(client.mosq);
    mosquitto_lib_cleanup();
    
    printf("✅ Disconnesso correttamente. Arrivederci!\n");
    return 0;
}