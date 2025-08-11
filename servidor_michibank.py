#!/usr/bin/env python3
"""
Servidor TCP para MichiBank con integracion MongoDB
Autor: MichiBank Team
Version: 1.0
Fecha: 2025-08-10

Este servidor maneja:
1. Conexiones TCP de clientes MichiBank
2. Operaciones CRUD en MongoDB
3. Chat entre clientes conectados
4. Menu principal de opciones
"""

import socket
import threading
import json
import time
from datetime import datetime
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, DuplicateKeyError

class MichiBankServer:
    def __init__(self, host='localhost', port=8888):
        self.host = host
        self.port = port
        self.server_socket = None
        self.clients = []
        self.chat_messages = []
        self.mongo_client = None
        self.db = None
        self.running = False
        
        # Configurar logging
        self.log_file = f"michibank_server_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
        
    def log(self, message):
        """Registra mensajes en archivo y consola"""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        log_entry = f"[{timestamp}] {message}"
        print(log_entry)
        
        try:
            with open(self.log_file, 'a', encoding='utf-8') as f:
                f.write(log_entry + '\n')
        except:
            pass
    
    def conectar_mongodb(self):
        """Conecta a MongoDB y crea las colecciones necesarias"""
        try:
            # Conectar a MongoDB (ajustar URI segun tu configuracion)
            self.mongo_client = MongoClient('mongodb://localhost:27017/')
            
            # Probar conexion
            self.mongo_client.admin.command('ping')
            
            # Seleccionar base de datos
            self.db = self.mongo_client['michibank']
            
            # Crear colecciones si no existen
            colecciones = ['titulares', 'cuentas', 'movimientos', 'titularCompleto']
            for coleccion in colecciones:
                if coleccion not in self.db.list_collection_names():
                    self.db.create_collection(coleccion)
            
            # Crear indices para optimizar busquedas
            self.db.titulares.create_index("cedula", unique=True)
            self.db.cuentas.create_index("id", unique=True)
            self.db.cuentas.create_index("cedulaTitular")
            self.db.movimientos.create_index("idCuenta")
            self.db.movimientos.create_index("fechaHora")
            self.db.titularCompleto.create_index("cedula", unique=True)
            
            self.log("Conexion a MongoDB establecida exitosamente")
            return True
            
        except ConnectionFailure as e:
            self.log(f"Error al conectar con MongoDB: {e}")
            return False
        except Exception as e:
            self.log(f"Error inesperado al configurar MongoDB: {e}")
            return False
    
    def verificar_mongodb(self):
        """Verifica si MongoDB esta disponible"""
        if self.mongo_client is None or self.db is None:
            return False
        
        try:
            self.mongo_client.admin.command('ping')
            return True
        except:
            return False
    
    def insertar_titular(self, data):
        """Inserta un nuevo titular en MongoDB"""
        try:
            titular_data = json.loads(data)
            titular_data['fechaRegistro'] = datetime.now().isoformat()
            
            result = self.db.titulares.insert_one(titular_data)
            self.log(f"Titular insertado: {titular_data['cedula']}")
            return "OK"
            
        except DuplicateKeyError:
            self.log(f"Titular ya existe: {titular_data.get('cedula', 'N/A')}")
            return "ERROR: Titular ya existe"
        except json.JSONDecodeError:
            self.log("Error al parsear JSON del titular")
            return "ERROR: JSON invalido"
        except Exception as e:
            self.log(f"Error al insertar titular: {e}")
            return "ERROR: Error de base de datos"
    
    def actualizar_titular(self, data):
        """Actualiza un titular existente"""
        try:
            titular_data = json.loads(data)
            cedula = titular_data.get('cedula')
            
            if not cedula:
                return "ERROR: Cedula requerida"
            
            titular_data['fechaActualizacion'] = datetime.now().isoformat()
            
            result = self.db.titulares.update_one(
                {'cedula': cedula},
                {'$set': titular_data}
            )
            
            if result.matched_count > 0:
                self.log(f"Titular actualizado: {cedula}")
                return "OK"
            else:
                return "ERROR: Titular no encontrado"
                
        except Exception as e:
            self.log(f"Error al actualizar titular: {e}")
            return "ERROR: Error de base de datos"
    
    def buscar_titular(self, cedula):
        """Busca un titular por cedula"""
        try:
            titular = self.db.titulares.find_one({'cedula': cedula})
            if titular:
                # Convertir ObjectId a string para JSON
                titular['_id'] = str(titular['_id'])
                return json.dumps(titular)
            else:
                return "NOT_FOUND"
                
        except Exception as e:
            self.log(f"Error al buscar titular: {e}")
            return "ERROR: Error de base de datos"
    
    def existe_titular(self, cedula):
        """Verifica si existe un titular con la cedula dada"""
        try:
            count = self.db.titulares.count_documents({'cedula': cedula})
            return "TRUE" if count > 0 else "FALSE"
        except Exception as e:
            self.log(f"Error al verificar titular: {e}")
            return "ERROR"
    
    def insertar_cuenta(self, cedula_titular, data):
        """Inserta una nueva cuenta bancaria"""
        try:
            cuenta_data = json.loads(data)
            cuenta_data['cedulaTitular'] = cedula_titular
            cuenta_data['fechaRegistro'] = datetime.now().isoformat()
            
            result = self.db.cuentas.insert_one(cuenta_data)
            self.log(f"Cuenta insertada: {cuenta_data['id']} para titular {cedula_titular}")
            return "OK"
            
        except DuplicateKeyError:
            return "ERROR: Cuenta ya existe"
        except Exception as e:
            self.log(f"Error al insertar cuenta: {e}")
            return "ERROR: Error de base de datos"
    
    def actualizar_saldo(self, id_cuenta, nuevo_saldo):
        """Actualiza el saldo de una cuenta"""
        try:
            result = self.db.cuentas.update_one(
                {'id': id_cuenta},
                {'$set': {
                    'saldo': float(nuevo_saldo),
                    'fechaActualizacion': datetime.now().isoformat()
                }}
            )
            
            if result.matched_count > 0:
                self.log(f"Saldo actualizado para cuenta {id_cuenta}: {nuevo_saldo}")
                return "OK"
            else:
                return "ERROR: Cuenta no encontrada"
                
        except Exception as e:
            self.log(f"Error al actualizar saldo: {e}")
            return "ERROR: Error de base de datos"
    
    def buscar_cuenta(self, id_cuenta):
        """Busca una cuenta por ID"""
        try:
            cuenta = self.db.cuentas.find_one({'id': id_cuenta})
            if cuenta:
                cuenta['_id'] = str(cuenta['_id'])
                return json.dumps(cuenta)
            else:
                return "NOT_FOUND"
                
        except Exception as e:
            self.log(f"Error al buscar cuenta: {e}")
            return "ERROR: Error de base de datos"
    
    def existe_cuenta(self, id_cuenta):
        """Verifica si existe una cuenta con el ID dado"""
        try:
            count = self.db.cuentas.count_documents({'id': id_cuenta})
            return "TRUE" if count > 0 else "FALSE"
        except Exception as e:
            self.log(f"Error al verificar cuenta: {e}")
            return "ERROR"
    
    def insertar_movimiento(self, id_cuenta, data):
        """Inserta un nuevo movimiento"""
        try:
            movimiento_data = json.loads(data)
            movimiento_data['idCuenta'] = id_cuenta
            movimiento_data['fechaRegistro'] = datetime.now().isoformat()
            
            result = self.db.movimientos.insert_one(movimiento_data)
            self.log(f"Movimiento insertado para cuenta {id_cuenta}")
            return "OK"
            
        except Exception as e:
            self.log(f"Error al insertar movimiento: {e}")
            return "ERROR: Error de base de datos"
    
    def buscar_movimientos_cuenta(self, id_cuenta):
        """Busca todos los movimientos de una cuenta"""
        try:
            movimientos = list(self.db.movimientos.find({'idCuenta': id_cuenta}))
            for mov in movimientos:
                mov['_id'] = str(mov['_id'])
            return json.dumps(movimientos)
            
        except Exception as e:
            self.log(f"Error al buscar movimientos: {e}")
            return "ERROR: Error de base de datos"
    
    def agregar_mensaje_chat(self, mensaje):
        """Agrega un mensaje al chat global"""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        mensaje_completo = f"[{timestamp}] {mensaje}"
        self.chat_messages.append(mensaje_completo)
        
        # Mantener solo los ultimos 100 mensajes
        if len(self.chat_messages) > 100:
            self.chat_messages = self.chat_messages[-100:]
        
        self.log(f"Mensaje de chat: {mensaje}")
        return "OK"
    
    def obtener_mensajes_chat(self):
        """Obtiene los mensajes de chat recientes"""
        return "\n".join(self.chat_messages[-10:])  # Ultimos 10 mensajes
    
    def insertar_titular_completo(self, data):
        """Inserta un titular completo con todas sus cuentas y movimientos"""
        try:
            titular_data = json.loads(data)
            titular_data['fechaCreacion'] = datetime.now().isoformat()
            titular_data['fechaActualizacion'] = datetime.now().isoformat()
            
            # Debug: Mostrar estructura recibida
            self.log(f"DEBUG - Datos recibidos para titular completo:")
            self.log(f"  Cedula: {titular_data.get('cedula', 'N/A')}")
            self.log(f"  Cuenta Corriente: {'Si' if titular_data.get('cuentaCorriente') else 'No'}")
            if titular_data.get('cuentaCorriente'):
                cc = titular_data['cuentaCorriente']
                movimientos_cc = cc.get('movimientos', [])
                self.log(f"    Movimientos en Cuenta Corriente: {len(movimientos_cc)}")
            
            self.log(f"  Cuentas Ahorro: {len(titular_data.get('cuentasAhorro', []))}")
            for i, cuenta in enumerate(titular_data.get('cuentasAhorro', [])):
                movimientos_ah = cuenta.get('movimientos', [])
                self.log(f"    Cuenta Ahorro {i+1} - Movimientos: {len(movimientos_ah)}")
            
            # Insertar o actualizar titular completo
            result = self.db.titularCompleto.replace_one(
                {'cedula': titular_data['cedula']},
                titular_data,
                upsert=True
            )
            
            self.log(f"Titular completo guardado: {titular_data['cedula']}")
            return "OK"
            
        except Exception as e:
            self.log(f"Error al insertar titular completo: {e}")
            return "ERROR: Error de base de datos"
    
    def actualizar_titular_completo(self, cedula, data):
        """Actualiza un titular completo"""
        try:
            titular_data = json.loads(data)
            titular_data['fechaActualizacion'] = datetime.now().isoformat()
            
            result = self.db.titularCompleto.update_one(
                {'cedula': cedula},
                {'$set': titular_data}
            )
            
            if result.matched_count > 0:
                self.log(f"Titular completo actualizado: {cedula}")
                return "OK"
            else:
                return "ERROR: Titular completo no encontrado"
                
        except Exception as e:
            self.log(f"Error al actualizar titular completo: {e}")
            return "ERROR: Error de base de datos"
    
    def buscar_titular_completo(self, cedula):
        """Busca un titular completo por cedula"""
        try:
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if titular:
                # Convertir ObjectId a string para JSON
                titular['_id'] = str(titular['_id'])
                return json.dumps(titular)
            else:
                return "NOT_FOUND"
                
        except Exception as e:
            self.log(f"Error al buscar titular completo: {e}")
            return "ERROR: Error de base de datos"
    
    def procesar_comando(self, comando):
        """Procesa un comando recibido del cliente"""
        try:
            if comando == "CHECK_MONGO":
                return "OK" if self.verificar_mongodb() else "ERROR"
            
            elif comando.startswith("EXISTS_TITULAR:"):
                cedula = comando.split(":", 1)[1]
                return self.existe_titular(cedula)
            
            elif comando.startswith("EXISTS_CUENTA:"):
                id_cuenta = comando.split(":", 1)[1]
                return self.existe_cuenta(id_cuenta)
            
            elif comando.startswith("INSERT_TITULAR:"):
                data = comando.split(":", 1)[1]
                return self.insertar_titular(data)
            
            elif comando.startswith("UPDATE_TITULAR:"):
                data = comando.split(":", 1)[1]
                return self.actualizar_titular(data)
            
            elif comando.startswith("FIND_TITULAR:"):
                cedula = comando.split(":", 1)[1]
                return self.buscar_titular(cedula)
            
            elif comando.startswith("INSERT_CUENTA:"):
                partes = comando.split(":", 2)
                cedula_titular = partes[1]
                data = partes[2]
                return self.insertar_cuenta(cedula_titular, data)
            
            elif comando.startswith("UPDATE_SALDO:"):
                partes = comando.split(":", 2)
                id_cuenta = partes[1]
                nuevo_saldo = partes[2]
                return self.actualizar_saldo(id_cuenta, nuevo_saldo)
            
            elif comando.startswith("FIND_CUENTA:"):
                id_cuenta = comando.split(":", 1)[1]
                return self.buscar_cuenta(id_cuenta)
            
            elif comando.startswith("INSERT_MOVIMIENTO:"):
                partes = comando.split(":", 2)
                id_cuenta = partes[1]
                data = partes[2]
                return self.insertar_movimiento(id_cuenta, data)
            
            elif comando.startswith("FIND_MOVIMIENTOS_CUENTA:"):
                id_cuenta = comando.split(":", 1)[1]
                return self.buscar_movimientos_cuenta(id_cuenta)
            
            elif comando.startswith("CHAT_SEND:"):
                mensaje = comando.split(":", 1)[1]
                return self.agregar_mensaje_chat(mensaje)
            
            elif comando == "CHAT_RECEIVE":
                return self.obtener_mensajes_chat()
            
            elif comando.startswith("INSERT_TITULAR_COMPLETO:"):
                data = comando.split(":", 1)[1]
                return self.insertar_titular_completo(data)
            
            elif comando.startswith("UPDATE_TITULAR_COMPLETO:"):
                partes = comando.split(":", 2)
                cedula = partes[1]
                data = partes[2]
                return self.actualizar_titular_completo(cedula, data)
            
            elif comando.startswith("FIND_TITULAR_COMPLETO:"):
                cedula = comando.split(":", 1)[1]
                return self.buscar_titular_completo(cedula)
            
            else:
                return "ERROR: Comando no reconocido"
                
        except Exception as e:
            self.log(f"Error al procesar comando '{comando}': {e}")
            return "ERROR: Error interno del servidor"
    
    def manejar_cliente(self, client_socket, address):
        """Maneja la comunicacion con un cliente"""
        self.log(f"Cliente conectado desde {address}")
        
        try:
            while self.running:
                # Recibir datos del cliente
                data = client_socket.recv(4096).decode('utf-8')
                if not data:
                    break
                
                # Procesar comando
                respuesta = self.procesar_comando(data.strip())
                
                # Enviar respuesta
                client_socket.send(respuesta.encode('utf-8'))
                
        except Exception as e:
            self.log(f"Error con cliente {address}: {e}")
        
        finally:
            client_socket.close()
            if client_socket in self.clients:
                self.clients.remove(client_socket)
            self.log(f"Cliente {address} desconectado")
    
    def iniciar_servidor(self):
        """Inicia el servidor TCP"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            
            self.running = True
            self.log(f"Servidor iniciado en {self.host}:{self.port}")
            
            while self.running:
                try:
                    client_socket, address = self.server_socket.accept()
                    self.clients.append(client_socket)
                    
                    # Crear hilo para manejar el cliente
                    client_thread = threading.Thread(
                        target=self.manejar_cliente,
                        args=(client_socket, address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error:
                    if self.running:
                        self.log("Error al aceptar conexion")
                    break
                    
        except Exception as e:
            self.log(f"Error al iniciar servidor: {e}")
        
        finally:
            self.detener_servidor()
    
    def detener_servidor(self):
        """Detiene el servidor"""
        self.running = False
        
        # Cerrar conexiones de clientes
        for client in self.clients:
            try:
                client.close()
            except:
                pass
        self.clients.clear()
        
        # Cerrar socket del servidor
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
        
        # Cerrar conexion MongoDB
        if self.mongo_client:
            try:
                self.mongo_client.close()
            except:
                pass
        
        self.log("Servidor detenido")

def mostrar_menu():
    """Muestra el menu principal"""
    print("\n" + "="*50)
    print("         MICHIBANK - SERVIDOR TCP/MONGODB")
    print("="*50)
    print("1. Ser servidor")
    print("2. Ser cliente")
    print("3. Chat")
    print("4. Salir")
    print("="*50)

def main():
    """Funcion principal"""
    servidor = None
    
    try:
        while True:
            mostrar_menu()
            opcion = input("Seleccione una opcion (1-4): ").strip()
            
            if opcion == "1":
                print("\n[MODO SERVIDOR]")
                print("Iniciando servidor MichiBank...")
                
                servidor = MichiBankServer()
                
                # Conectar a MongoDB
                if servidor.conectar_mongodb():
                    print("MongoDB conectado exitosamente")
                    print(f"Colecciones disponibles: {servidor.db.list_collection_names()}")
                else:
                    print("ADVERTENCIA: MongoDB no disponible. Funcionalidad limitada.")
                
                print(f"Servidor escuchando en {servidor.host}:{servidor.port}")
                print("Presione Ctrl+C para detener el servidor")
                
                try:
                    servidor.iniciar_servidor()
                except KeyboardInterrupt:
                    print("\nDeteniendo servidor...")
                    if servidor:
                        servidor.detener_servidor()
            
            elif opcion == "2":
                print("\n[MODO CLIENTE]")
                ip_servidor = input("IP del servidor (localhost): ").strip()
                if not ip_servidor:
                    ip_servidor = "localhost"
                
                try:
                    puerto = int(input("Puerto del servidor (8888): ").strip() or "8888")
                except ValueError:
                    puerto = 8888
                
                print(f"Conectando a {ip_servidor}:{puerto}...")
                print("NOTA: En modo cliente, debe ejecutar la aplicacion MichiBank en C++")
                print("La aplicacion C++ se conectara automaticamente al servidor")
                
                input("Presione Enter para continuar...")
            
            elif opcion == "3":
                print("\n[MODO CHAT]")
                ip_servidor = input("IP del servidor (localhost): ").strip()
                if not ip_servidor:
                    ip_servidor = "localhost"
                
                try:
                    puerto = int(input("Puerto del servidor (8888): ").strip() or "8888")
                except ValueError:
                    puerto = 8888
                
                # Implementar cliente de chat simple
                try:
                    chat_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    chat_socket.connect((ip_servidor, puerto))
                    
                    print(f"Conectado al chat en {ip_servidor}:{puerto}")
                    print("Escriba 'salir' para terminar el chat")
                    
                    while True:
                        mensaje = input("Mensaje: ").strip()
                        if mensaje.lower() == 'salir':
                            break
                        
                        if mensaje:
                            # Enviar mensaje
                            comando = f"CHAT_SEND:{mensaje}"
                            chat_socket.send(comando.encode('utf-8'))
                            respuesta = chat_socket.recv(1024).decode('utf-8')
                            
                            if respuesta == "OK":
                                print("Mensaje enviado")
                            else:
                                print(f"Error: {respuesta}")
                            
                            # Recibir mensajes
                            chat_socket.send("CHAT_RECEIVE".encode('utf-8'))
                            mensajes = chat_socket.recv(4096).decode('utf-8')
                            if mensajes:
                                print("\n--- Mensajes recientes ---")
                                print(mensajes)
                                print("-------------------------\n")
                    
                    chat_socket.close()
                    
                except Exception as e:
                    print(f"Error en chat: {e}")
            
            elif opcion == "4":
                print("Saliendo del programa...")
                break
            
            else:
                print("Opcion no valida. Intente nuevamente.")
    
    except KeyboardInterrupt:
        print("\nPrograma interrumpido por el usuario")
    
    except Exception as e:
        print(f"Error inesperado: {e}")
    
    finally:
        if servidor:
            servidor.detener_servidor()

if __name__ == "__main__":
    print("MichiBank TCP/MongoDB Server v1.0")
    print("Requisitos: pip install pymongo")
    
    # Verificar dependencias
    try:
        import pymongo
        print("PyMongo encontrado - OK")
    except ImportError:
        print("ERROR: PyMongo no instalado")
        print("Instale con: pip install pymongo")
        exit(1)
    
    main()
