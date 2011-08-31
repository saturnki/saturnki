using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace ConsoleApplication1
{
    class Program
    {
        static SerialPort serial_port;

        enum Packets
        {
            kFirst,
            kMiddle,
            kLast
        };

        static byte[] RequestBytes(
            Packets which_packet,
            byte data_length,
            int source_address)
        {
            byte[] packet = new byte[9];

            packet[0] = 0x5a;
            packet[1] = 0x7;

            if (which_packet == Packets.kFirst)
                packet[2] = 0x01;
            else if (which_packet == Packets.kMiddle)
                packet[2] = 0x11;
            else
                packet[2] = 0x21;

            packet[3] = (byte)((source_address >> 24) & 0xFF);
            packet[4] = (byte)((source_address >> 16) & 0xFF);
            packet[5] = (byte)((source_address >> 8) & 0xFF);
            packet[6] = (byte)(source_address & 0xFF);
            packet[7] = data_length;

            byte checksum = 0;
            for (int i = 1; i < 8; i++)
            {
                checksum += packet[i];
            }

            packet[8] = checksum;

            serial_port.Write(packet, 0, 9);

            System.Threading.Thread.Sleep(20);

            int response_size = data_length + 7 + 1 + 1;
            byte[] response = new byte[response_size];

            serial_port.Read(response, 0, response_size);

            if (response[2] != 0xff)
                throw new Exception("Error with request");

            if (response[1] != (byte)(data_length + 0x07))
                throw new Exception("Data length is wrong");

            return response;
        }

        static void Read()
        {
            int length = 524288 / 4096;
            int origin = 0x01000000;
            int current_address = origin;

            byte[] result = new byte[length];

            Packets packet = Packets.kFirst;

            byte chunk_length = 8;
            int number_of_chunks = length / chunk_length;

            for (int i = 0; i < number_of_chunks; i++)
            {
                if (packet == Packets.kFirst)
                {
                    byte[] temp = RequestBytes(Packets.kFirst, chunk_length, current_address);
                    //we have to skip first 8 bytes and the last one
                    System.Buffer.BlockCopy(temp, 8, result, current_address - origin, chunk_length);
                    packet = Packets.kMiddle;
                }
                else if (packet == Packets.kMiddle)
                {
                    byte[] temp = RequestBytes(Packets.kMiddle, chunk_length, current_address);
                    System.Buffer.BlockCopy(temp, 8, result, current_address - origin, chunk_length);
                    if (i == number_of_chunks - 1)
                        packet = Packets.kLast;
                }
                else
                {
                    byte[] temp = RequestBytes(Packets.kLast, chunk_length, current_address);
                    System.Buffer.BlockCopy(temp, 8, result, current_address - origin, chunk_length);
                }

                current_address += chunk_length;
            }
        }

        static void SendDataPacket(
             byte[] data_to_send,
             int destination_address,
             bool jump_to_address)
        {
            byte data_length = (byte)data_to_send.Length;

            byte packet_size = (byte)(data_length + 7);
            //1 initial byte, 1 checksum byte
            byte[] data = new byte[packet_size + 1 + 1];

            data[0] = 0x5a;
            data[1] = packet_size;

            if (jump_to_address)
                data[2] = 0x19;
            else
                data[2] = 0x09;

            data[3] = (byte)((destination_address >> 24) & 0xFF);
            data[4] = (byte)((destination_address >> 16) & 0xFF);
            data[5] = (byte)((destination_address >> 8) & 0xFF);
            data[6] = (byte)(destination_address & 0xFF);
            data[7] = data_length;

            for (int i = 0; i < data_length; i++)
            {
                data[i + 8] = data_to_send[i];
            }

            byte checksum = 0;
            for (int i = 1; i < packet_size + 1; i++)
            {
                checksum += data[i];
            }

            data[packet_size + 1] = checksum;

            serial_port.Write(data, 0, packet_size + 1 + 1);

            System.Threading.Thread.Sleep(20);

            byte[] response = new byte[9];
            serial_port.Read(response, 0, 9);

            if (response[2] == 0x00)
                throw new Exception("Error with transmission");
        }

        static byte[] Chunkify(
            byte[] file,
            int chunk_offset,
            int chunk_size)
        {
            byte[] result = new byte[chunk_size];

            for (int j = 0; j < chunk_size; j++)
            {
                int main_file_index = (chunk_offset * chunk_size) + j;
                if (main_file_index >= file.Length)
                    result[j] = 0;
                else
                    result[j] = file[main_file_index];
            }
            return result;
        }

        static void Write(string filename)
        {
            int origin = 0x06004000;
            byte[] file = System.IO.File.ReadAllBytes(filename);

            int chunk_size = 191;

            int number_of_chunks = (file.Length / chunk_size) + 1;

            for (int i = 1; i < number_of_chunks; i++)
            {
                byte[] data_to_send = Chunkify(file, i, chunk_size);
                SendDataPacket(data_to_send, origin + (i * chunk_size), false);
            }

            byte[] final = Chunkify(file, 0, chunk_size);
            SendDataPacket(final, origin, true);
        }
        static void TestChunkifier()
        {
            byte[] file = System.IO.File.ReadAllBytes(@"C:\SaturnOrbit\EXAMPLES\CharlesMacDonald\exbk\main_coff.bin");
            int chunk_size = 100;
            int number_of_chunks = file.Length / chunk_size;
            byte[][] chunks = new byte[number_of_chunks][];

            for (int x = 0; x < chunks.Length; x++)
            {
                chunks[x] = new byte[chunk_size];
            }

            //fill our array with the chunked data
            for (int y = 0; y < number_of_chunks; y++)
            {
                byte[] c = Chunkify(file, y, chunk_size);

                for (int x = 0; x < chunk_size; x++)
                {
                    chunks[y][x] = c[x];
                }
            }

            int i = 0;

            for (int y = 0; y < number_of_chunks; y++)
            {
                for (int x = 0; x < chunk_size; x++)
                {
                    if (file[i++] != chunks[y][x])
                        throw new Exception("Chunker is broken");
                }
            }

            if (i <= file.Length - 1)
                throw new Exception("You didn't make it to the end of the file");

        }
        static void Main(string[] args)
        {

            //   TestChunkifier();

            serial_port = new SerialPort("COM4");
            serial_port.BaudRate = 375000;
            serial_port.Parity = Parity.None;
            serial_port.DataBits = 8;
            serial_port.StopBits = StopBits.Two;
            serial_port.Handshake = Handshake.None;
            serial_port.ReadTimeout = 200;

            serial_port.Open();

            if (serial_port.IsOpen)
            {
                //                byte[] test = {0,0,2,0,0};
                //                SendDataPacket(test, 0x06004000, false);
                //@"C:\SaturnOrbit\EXAMPLES\CharlesMacDonald\exbk\main_coff.bin"
                //  Write(@args[0]);
                Write(@"C:\svn\test\tests\hardware_level\main_coff.bin");
                // Read();
            }
        }
    }
}
