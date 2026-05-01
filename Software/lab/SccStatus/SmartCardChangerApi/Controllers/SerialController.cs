using Microsoft.AspNetCore.Mvc;
using System.IO.Ports;

namespace SmartCardChangerApi.Controllers;

[Route("api/[controller]")]
[ApiController]
public class SerialController : ControllerBase
{

  // GET: api/serial/ports
  [HttpGet("ports")]
  public IActionResult GetPorts()
  {
    try
    {
      string[] ports = SerialPort.GetPortNames();

      return Ok(new
      {
        success = true,
        ports = ports,
        count = ports.Length
      });
    }
    catch (Exception ex)
    {
      return StatusCode(500, new
      {
        success = false,
        error = ex.Message
      });
    }
  }

  // PATCH: api/serial/ports
  [HttpPatch("ports/{comPort}")]
  public IActionResult Patch(string comPort, [FromBody] CommandRequest request)
  {
    try
    {
      // Open the first available port
      SerialPort serialPort = new SerialPort(comPort, 9600);
      serialPort.ReadTimeout = 2000;
      serialPort.Handshake = Handshake.None;
      serialPort.DtrEnable = true;
      serialPort.RtsEnable = true;
      serialPort.Open();

      Console.WriteLine($"Connected to {comPort} {serialPort.IsOpen}");

      try
      {
        // Send/receive data here

        // Send the query
        serialPort.WriteLine(request.Command);
        Console.WriteLine($"Sent: {request.Command}");

        // Wait a bit for the device to respond
        Thread.Sleep(500);

        if (serialPort.BytesToRead <= 0)
        {
          return Ok(new
          {
            success = false,
          });
        }
        //byte[] buffer = new byte[30];
        //int bytesRead = serialPort.Read(buffer, 0, 30);
        //string response = System.Text.Encoding.ASCII.GetString(buffer, 0, bytesRead);

        // Receive the response
        string serialoutput = serialPort.ReadExisting();
        Console.WriteLine($"Received: {serialoutput}");

        return Ok(new
        {
          success = true,
          response = serialoutput
        });
      }
      finally
      {
        Console.WriteLine("Close connection");
        serialPort.Close();
      }
    }
    catch (Exception ex)
    {
      return StatusCode(500, new
      {
        success = false,
        error = ex.Message
      });
    }
  }
}

public record CommandRequest(string Command)
{
}

public record CommandResponse(double Result)
{
}

