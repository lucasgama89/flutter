// Main entry for the TIorienta application
import 'package:flutter/material.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';

double initialLatitude = -1.455833; // Latitude de Belém
double initialLongitude = -48.504444; // Longitude de Belém

void main() {
  runApp(TIorientaApp());
}

class TIorientaApp extends StatelessWidget {
  const TIorientaApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'TIorienta',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: HomeScreen(),
    );
  }
}

class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('TIorienta'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text(
              'Bem-vindo ao TIorienta!',
              style: TextStyle(fontSize: 24),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => RoutesScreen()),
                );
              },
              child: Text('Ver Horários de Ônibus'),
            ),
            ElevatedButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => MapScreen(initialLatitude: initialLatitude, initialLongitude: initialLongitude)),
                );
              },
              child: Text('Ver Mapa e Rotas'),
            ),
          ],
        ),
      ),
    );
  }
}

class RoutesScreen extends StatelessWidget {
  const RoutesScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final busSchedules = [
      {'linha': 'A001', 'horario': '06:00 - 22:00'},
      {'linha': 'A002', 'horario': '05:30 - 21:30'},
      {'linha': 'A003', 'horario': '06:15 - 22:15'},
    ];

    return Scaffold(
      appBar: AppBar(
        title: Text('Horários de Ônibus'),
      ),
      body: ListView.builder(
        itemCount: busSchedules.length,
        itemBuilder: (context, index) {
          final bus = busSchedules[index];
          return ListTile(
            title: Text('Linha: ${bus['linha']}'),
            subtitle: Text('Horário: ${bus['horario']}'),
          );
        },
      ),
    );
  }
}

class MapScreen extends StatelessWidget {
  final double initialLatitude;
  final double initialLongitude;

  const MapScreen({super.key, required this.initialLatitude, required this.initialLongitude});

  @override
  Widget build(BuildContext context) {
    Set<Marker> busStops = {
      Marker(
        markerId: MarkerId('busStop1'),
        position: LatLng(-1.455833, -48.504444),
        infoWindow: InfoWindow(title: 'Ponto de Ônibus 1'),
      ),
      Marker(
        markerId: MarkerId('busStop2'),
        position: LatLng(-1.456500, -48.500000),
        infoWindow: InfoWindow(title: 'Ponto de Ônibus 2'),
      ),
      Marker(
        markerId: MarkerId('busStop3'),
        position: LatLng(-1.460000, -48.505000),
        infoWindow: InfoWindow(title: 'Ponto de Ônibus 3'),
      ),
    };

    return Scaffold(
      appBar: AppBar(
        title: Text('Mapa e Rotas'),
      ),
      body: GoogleMap(
        initialCameraPosition: CameraPosition(
          target: LatLng(initialLatitude, initialLongitude),
          zoom: 12,
        ),
        markers: busStops,
      ),
    );
  }
}
