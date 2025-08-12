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
    def __init__(self, host='0.0.0.0', port=8888):
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
    
    def depositar_atomico(self, cedula, id_cuenta, monto, movimiento_data):
        """Realiza un deposito atomico sin riesgo de concurrencia"""
        try:
            movimiento = json.loads(movimiento_data)
            
            # Determinar si es cuenta corriente o ahorro
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if not titular:
                return "ERROR: Titular no encontrado"
            
            es_cuenta_corriente = (titular.get('cuentaCorriente') and 
                                 titular['cuentaCorriente'].get('id') == id_cuenta)
            
            if es_cuenta_corriente:
                # Actualizar cuenta corriente atomicamente
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula, "cuentaCorriente.id": id_cuenta},
                    {
                        "$inc": {"cuentaCorriente.saldo": float(monto)},
                        "$push": {"cuentaCorriente.movimientos": movimiento},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
            else:
                # Actualizar cuenta de ahorro atomicamente
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula, "cuentasAhorro.id": id_cuenta},
                    {
                        "$inc": {"cuentasAhorro.$.saldo": float(monto)},
                        "$push": {"cuentasAhorro.$.movimientos": movimiento},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
            
            if result.matched_count > 0:
                # Actualizar tambien la coleccion de cuentas independiente
                self.db.cuentas.update_one(
                    {"id": id_cuenta},
                    {
                        "$inc": {"saldo": float(monto)},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
                
                # Insertar movimiento en coleccion independiente
                movimiento["idCuenta"] = id_cuenta
                movimiento["fechaCreacion"] = datetime.now().isoformat()
                self.db.movimientos.insert_one(movimiento)
                
                self.log(f"Deposito atomico exitoso: ${monto} en cuenta {id_cuenta}")
                return "OK"
            else:
                return "ERROR: No se pudo actualizar la cuenta"
                
        except Exception as e:
            self.log(f"Error en deposito atomico: {e}")
            return "ERROR: Error de base de datos"
    
    def retirar_atomico(self, cedula, id_cuenta, monto, movimiento_data):
        """Realiza un retiro atomico sin riesgo de concurrencia"""
        try:
            movimiento = json.loads(movimiento_data)
            
            # Verificar saldo antes del retiro
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if not titular:
                return "ERROR: Titular no encontrado"
            
            saldo_actual = 0
            es_cuenta_corriente = (titular.get('cuentaCorriente') and 
                                 titular['cuentaCorriente'].get('id') == id_cuenta)
            
            if es_cuenta_corriente:
                saldo_actual = titular['cuentaCorriente'].get('saldo', 0)
            else:
                for cuenta in titular.get('cuentasAhorro', []):
                    if cuenta.get('id') == id_cuenta:
                        saldo_actual = cuenta.get('saldo', 0)
                        break
            
            if saldo_actual < float(monto):
                return "ERROR: Saldo insuficiente"
            
            # Realizar retiro atomico
            if es_cuenta_corriente:
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula, "cuentaCorriente.id": id_cuenta},
                    {
                        "$inc": {"cuentaCorriente.saldo": -float(monto)},
                        "$push": {"cuentaCorriente.movimientos": movimiento},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
            else:
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula, "cuentasAhorro.id": id_cuenta},
                    {
                        "$inc": {"cuentasAhorro.$.saldo": -float(monto)},
                        "$push": {"cuentasAhorro.$.movimientos": movimiento},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
            
            if result.matched_count > 0:
                # Actualizar tambien la coleccion de cuentas independiente
                self.db.cuentas.update_one(
                    {"id": id_cuenta},
                    {
                        "$inc": {"saldo": -float(monto)},
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
                
                # Insertar movimiento en coleccion independiente
                movimiento["idCuenta"] = id_cuenta
                movimiento["fechaCreacion"] = datetime.now().isoformat()
                self.db.movimientos.insert_one(movimiento)
                
                self.log(f"Retiro atomico exitoso: ${monto} de cuenta {id_cuenta}")
                return "OK"
            else:
                return "ERROR: No se pudo actualizar la cuenta"
                
        except Exception as e:
            self.log(f"Error en retiro atomico: {e}")
            return "ERROR: Error de base de datos"
    
    def obtener_saldo_atomico(self, cedula, id_cuenta):
        """Obtiene el saldo actual de una cuenta de forma atomica"""
        try:
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if not titular:
                return "ERROR: Titular no encontrado"
            
            # Buscar en cuenta corriente
            if (titular.get('cuentaCorriente') and 
                titular['cuentaCorriente'].get('id') == id_cuenta):
                saldo = titular['cuentaCorriente'].get('saldo', 0)
                return str(saldo)
            
            # Buscar en cuentas de ahorro
            for cuenta in titular.get('cuentasAhorro', []):
                if cuenta.get('id') == id_cuenta:
                    saldo = cuenta.get('saldo', 0)
                    return str(saldo)
            
            return "ERROR: Cuenta no encontrada"
            
        except Exception as e:
            self.log(f"Error al obtener saldo atomico: {e}")
            return "ERROR: Error de base de datos"
    
    def crear_cuenta_atomica(self, cedula, tipo_cuenta, cuenta_data):
        """Crea una cuenta atomicamente sin riesgo de concurrencia"""
        try:
            cuenta = json.loads(cuenta_data)
            
            # Verificar que el titular existe
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if not titular:
                return "ERROR: Titular no encontrado"
            
            if tipo_cuenta == "CORRIENTE":
                # Verificar que no tenga cuenta corriente ya
                if titular.get('cuentaCorriente') is not None:
                    return "ERROR: Ya tiene cuenta corriente"
                
                # Crear cuenta corriente atomicamente
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula, "cuentaCorriente": None},  # Solo si no tiene
                    {
                        "$set": {
                            "cuentaCorriente": cuenta,
                            "fechaActualizacion": datetime.now().isoformat()
                        }
                    }
                )
                
                if result.matched_count > 0:
                    # Tambien insertar en coleccion independiente de cuentas
                    cuenta["cedulaTitular"] = cedula
                    cuenta["fechaRegistro"] = datetime.now().isoformat()
                    self.db.cuentas.insert_one(cuenta)
                    
                    self.log(f"Cuenta corriente creada atomicamente para titular {cedula}")
                    return "OK"
                else:
                    return "ERROR: Ya tiene cuenta corriente o titular no encontrado"
                    
            elif tipo_cuenta == "AHORRO":
                # Agregar a array de cuentas de ahorro
                result = self.db.titularCompleto.update_one(
                    {"cedula": cedula},
                    {
                        "$push": {"cuentasAhorro": cuenta},  # $push nunca sobrescribe
                        "$set": {"fechaActualizacion": datetime.now().isoformat()}
                    }
                )
                
                if result.matched_count > 0:
                    # Tambien insertar en coleccion independiente de cuentas
                    cuenta["cedulaTitular"] = cedula
                    cuenta["fechaRegistro"] = datetime.now().isoformat()
                    self.db.cuentas.insert_one(cuenta)
                    
                    self.log(f"Cuenta de ahorro creada atomicamente para titular {cedula}")
                    return "OK"
                else:
                    return "ERROR: Titular no encontrado"
            else:
                return "ERROR: Tipo de cuenta invalido"
                
        except DuplicateKeyError:
            return "ERROR: La cuenta ya existe"
        except Exception as e:
            self.log(f"Error en creacion atomica de cuenta: {e}")
            return "ERROR: Error de base de datos"
    
    def verificar_cuenta_corriente(self, cedula):
        """Verifica si un titular ya tiene cuenta corriente"""
        try:
            titular = self.db.titularCompleto.find_one({'cedula': cedula})
            if not titular:
                return "ERROR: Titular no encontrado"
            
            tiene_corriente = titular.get('cuentaCorriente') is not None
            return "TRUE" if tiene_corriente else "FALSE"
            
        except Exception as e:
            self.log(f"Error al verificar cuenta corriente: {e}")
            return "ERROR: Error de base de datos"
    
    def obtener_todos_titulares(self):
        """Obtiene todos los titulares completos de la base de datos"""
        try:
            titulares = list(self.db.titularCompleto.find({}))
            if titulares:
                # Convertir ObjectId a string para JSON
                for titular in titulares:
                    titular['_id'] = str(titular['_id'])
                
                self.log(f"Enviando {len(titulares)} titulares completos al cliente")
                return json.dumps(titulares)
            else:
                self.log("No se encontraron titulares en la base de datos")
                return "[]"  # Array vacío JSON
                
        except Exception as e:
            self.log(f"Error al obtener todos los titulares: {e}")
            return "ERROR: Error de base de datos"
    
    def exportar_base_datos_completa(self, nombre_archivo):
        """Exporta toda la base de datos a un archivo JSON"""
        try:
            # Crear diccionario con todas las colecciones
            backup_data = {}
            
            # Exportar colección titulares
            titulares = list(self.db.titulares.find({}))
            for titular in titulares:
                titular['_id'] = str(titular['_id'])
            backup_data['titulares'] = titulares
            
            # Exportar colección cuentas
            cuentas = list(self.db.cuentas.find({}))
            for cuenta in cuentas:
                cuenta['_id'] = str(cuenta['_id'])
            backup_data['cuentas'] = cuentas
            
            # Exportar colección movimientos
            movimientos = list(self.db.movimientos.find({}))
            for movimiento in movimientos:
                movimiento['_id'] = str(movimiento['_id'])
            backup_data['movimientos'] = movimientos
            
            # Exportar colección titularCompleto
            titularCompleto = list(self.db.titularCompleto.find({}))
            for titular in titularCompleto:
                titular['_id'] = str(titular['_id'])
            backup_data['titularCompleto'] = titularCompleto
            
            # Exportar colección chat (si existe)
            try:
                mensajes = list(self.db.chat.find({}))
                for mensaje in mensajes:
                    mensaje['_id'] = str(mensaje['_id'])
                backup_data['chat'] = mensajes
            except:
                backup_data['chat'] = []
            
            # Metadata del backup
            backup_data['metadata'] = {
                'fecha_backup': datetime.now().isoformat(),
                'version': '1.0',
                'total_titulares': len(titulares),
                'total_cuentas': len(cuentas),
                'total_movimientos': len(movimientos),
                'total_titularCompleto': len(titularCompleto),
                'total_mensajes': len(backup_data['chat'])
            }
            
            # Guardar en archivo JSON
            with open(nombre_archivo, 'w', encoding='utf-8') as f:
                json.dump(backup_data, f, ensure_ascii=False, indent=2)
            
            self.log(f"Backup completo creado: {nombre_archivo}")
            self.log(f"Titulares exportados: {len(titulares)}")
            self.log(f"Cuentas exportadas: {len(cuentas)}")
            self.log(f"Movimientos exportados: {len(movimientos)}")
            self.log(f"TitularCompleto exportados: {len(titularCompleto)}")
            self.log(f"Mensajes exportados: {len(backup_data['chat'])}")
            
            return "SUCCESS: Backup de base de datos creado exitosamente"
            
        except Exception as e:
            self.log(f"Error al exportar base de datos: {e}")
            return f"ERROR: Error al crear backup - {str(e)}"
    
    def importar_base_datos_completa(self, nombre_archivo):
        """Importa toda la base de datos desde un archivo JSON"""
        try:
            # Verificar que el archivo existe
            import os
            if not os.path.exists(nombre_archivo):
                return f"ERROR: Archivo {nombre_archivo} no encontrado"
            
            # Leer archivo JSON
            with open(nombre_archivo, 'r', encoding='utf-8') as f:
                backup_data = json.load(f)
            
            # Validar estructura básica del backup
            if 'metadata' not in backup_data:
                return "ERROR: Archivo de backup invalido - falta metadata"
            
            # ADVERTENCIA: Esta operación borra toda la base de datos actual
            self.log("ATENCION: Eliminando todos los datos actuales...")
            
            # Limpiar TODAS las colecciones existentes
            self.db.titulares.delete_many({})
            self.db.cuentas.delete_many({})
            self.db.movimientos.delete_many({})
            self.db.titularCompleto.delete_many({})
            try:
                self.db.chat.delete_many({})
            except:
                pass  # La colección chat puede no existir
            
            # Contadores para el reporte
            titulares_importados = 0
            cuentas_importadas = 0
            movimientos_importados = 0
            titularCompleto_importados = 0
            mensajes_importados = 0
            
            # Restaurar colección titulares
            if 'titulares' in backup_data and backup_data['titulares']:
                for titular in backup_data['titulares']:
                    # Remover _id para que MongoDB genere uno nuevo
                    if '_id' in titular:
                        del titular['_id']
                
                resultado = self.db.titulares.insert_many(backup_data['titulares'])
                titulares_importados = len(resultado.inserted_ids)
            
            # Restaurar colección cuentas
            if 'cuentas' in backup_data and backup_data['cuentas']:
                for cuenta in backup_data['cuentas']:
                    # Remover _id para que MongoDB genere uno nuevo
                    if '_id' in cuenta:
                        del cuenta['_id']
                
                resultado = self.db.cuentas.insert_many(backup_data['cuentas'])
                cuentas_importadas = len(resultado.inserted_ids)
            
            # Restaurar colección movimientos
            if 'movimientos' in backup_data and backup_data['movimientos']:
                for movimiento in backup_data['movimientos']:
                    # Remover _id para que MongoDB genere uno nuevo
                    if '_id' in movimiento:
                        del movimiento['_id']
                
                resultado = self.db.movimientos.insert_many(backup_data['movimientos'])
                movimientos_importados = len(resultado.inserted_ids)
            
            # Restaurar colección titularCompleto
            if 'titularCompleto' in backup_data and backup_data['titularCompleto']:
                for titular in backup_data['titularCompleto']:
                    # Remover _id para que MongoDB genere uno nuevo
                    if '_id' in titular:
                        del titular['_id']
                
                resultado = self.db.titularCompleto.insert_many(backup_data['titularCompleto'])
                titularCompleto_importados = len(resultado.inserted_ids)
            
            # Restaurar chat (si existe en el backup)
            if 'chat' in backup_data and backup_data['chat']:
                for mensaje in backup_data['chat']:
                    # Remover _id para que MongoDB genere uno nuevo
                    if '_id' in mensaje:
                        del mensaje['_id']
                
                resultado = self.db.chat.insert_many(backup_data['chat'])
                mensajes_importados = len(resultado.inserted_ids)
            
            # Log del resultado
            self.log(f"Restauracion completa desde: {nombre_archivo}")
            self.log(f"Titulares restaurados: {titulares_importados}")
            self.log(f"Cuentas restauradas: {cuentas_importadas}")
            self.log(f"Movimientos restaurados: {movimientos_importados}")
            self.log(f"TitularCompleto restaurados: {titularCompleto_importados}")
            self.log(f"Mensajes restaurados: {mensajes_importados}")
            
            if 'metadata' in backup_data:
                metadata = backup_data['metadata']
                self.log(f"Backup original del: {metadata.get('fecha_backup', 'N/A')}")
            
            return f"SUCCESS: Base de datos restaurada - {titulares_importados} titulares, {cuentas_importadas} cuentas, {movimientos_importados} movimientos, {titularCompleto_importados} titularCompleto, {mensajes_importados} mensajes"
            
        except Exception as e:
            self.log(f"Error al importar base de datos: {e}")
            return f"ERROR: Error al restaurar backup - {str(e)}"
    
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
            
            elif comando.startswith("DEPOSITAR_ATOMICO:"):
                partes = comando.split(":", 4)
                cedula = partes[1]
                id_cuenta = partes[2]
                monto = partes[3]
                movimiento_data = partes[4]
                return self.depositar_atomico(cedula, id_cuenta, monto, movimiento_data)
            
            elif comando.startswith("RETIRAR_ATOMICO:"):
                partes = comando.split(":", 4)
                cedula = partes[1]
                id_cuenta = partes[2]
                monto = partes[3]
                movimiento_data = partes[4]
                return self.retirar_atomico(cedula, id_cuenta, monto, movimiento_data)
            
            elif comando.startswith("OBTENER_SALDO_ATOMICO:"):
                partes = comando.split(":", 2)
                cedula = partes[1]
                id_cuenta = partes[2]
                return self.obtener_saldo_atomico(cedula, id_cuenta)
            
            elif comando.startswith("CREAR_CUENTA_ATOMICA:"):
                partes = comando.split(":", 3)
                cedula = partes[1]
                tipo_cuenta = partes[2]
                cuenta_data = partes[3]
                return self.crear_cuenta_atomica(cedula, tipo_cuenta, cuenta_data)
            
            elif comando.startswith("VERIFICAR_CUENTA_CORRIENTE:"):
                cedula = comando.split(":", 1)[1]
                return self.verificar_cuenta_corriente(cedula)
            
            elif comando == "GET_ALL_TITULARES":
                return self.obtener_todos_titulares()
            
            elif comando.startswith("EXPORT_DATABASE:"):
                nombre_archivo = comando.split(":", 1)[1]
                return self.exportar_base_datos_completa(nombre_archivo)
            
            elif comando.startswith("IMPORT_DATABASE:"):
                nombre_archivo = comando.split(":", 1)[1]
                return self.importar_base_datos_completa(nombre_archivo)
            
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
