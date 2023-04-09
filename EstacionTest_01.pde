import processing.serial.*;

Serial puerto;//establece variable puerto tipo Serial
PrintWriter fichero;//establece variable fichero tipo PrintWriter (fichero)
int contador=0;//variable para contar el nº de grabaciones realizadas

void setup() {
  //println(Serial.list());
  size (400, 335);
  background(0);
  puerto = new Serial(this, Serial.list()[32], 9600);//establece la variable puerto asignando el receptor APC200 y una velocidad de 9600 baudios
  fichero=createWriter("datos.csv");//crea el fichero datos.csv
  datosPantalla("Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos");
}

void draw() {

  datosPantalla("Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos");

  while (puerto.available()>0) {//mientras haya datos en el puerto serie
    String buffer=puerto.readStringUntil(10);//leemos los datos del puerto serie hasta el salto de línea (10 en ascii), y los asignamos a buffer

    if (buffer!=null) {//si buffer no está vacío
      String[] listaBuffer = split(buffer, ',');//extrae en un array los elementos separados por coma del buffer
      int longitudBuffer=buffer.length();//leemos el nº de caracteres del buffer
      int numeroCampos=listaBuffer.length;//leemos el nº de campos que están en el array, deben ser 10
      //println (buffer);
      println ("el nº de campos es:" + numeroCampos + " el nº de caracteres es:" + longitudBuffer);//imprimimos en consola el nº de campos y nº de caracteres recibidos

      if (numeroCampos==10 && listaBuffer[0].equals("soto")==true && listaBuffer[9].equals("fin\r\n")==true) {//grabamos si hay 10 campos y los campos de control de inicio y fin son correctos
        contador++;
        fichero.print(buffer);//colocamos en el fichero el buffer
        fichero.flush();//hacemos la grabación efectiva y se cierra el fichero
        println(buffer);//imprimimos por consola lo grabado
        datosPantalla(listaBuffer[1], listaBuffer[2], listaBuffer[3], listaBuffer[4], listaBuffer[5], listaBuffer[6], listaBuffer[7], listaBuffer[8]);//llamamos a la función que nos pone en pantalla los datos recogidos
      } else {
        datosPantalla("Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos", "Sin datos");
      }
    }
  }
}

void datosPantalla(String tiempo, String longitud, String latitud, String altitud, String velocidad, String temperatura, String presion, String acelz) {
  background(0);
  textSize (24);
  text ("Datos SotoSat", 120, 35);
  textSize(16);
  text ("Tiempo(s):"+ tiempo, 50, 75);
  text ("Longitud:"+longitud, 50, 100);
  text ("Latitud:"+latitud, 50, 125);
  text ("Altitud (m):"+altitud, 50, 150);
  //text ("Rumbo:"+rumbo, 50, 175);
  text ("Velocidad (Km/h):"+velocidad, 50, 175);
  text ("Temperatura (ºC):"+temperatura, 50, 200);
  text ("Presión (mb):"+presion, 50, 225);
  text ("Acel. z (m/s2):"+acelz, 50, 250);
  //text ("Día:"+dia, 50, 100);
  //text ("Hora:"+hora, 50, 125);
}
