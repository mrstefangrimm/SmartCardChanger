using SmartCardChangerApi.Controllers;
using System.IO.Ports;

namespace SmartCardChangerApi.Services;

public interface ISerialService
{
  string[] GetPorts();
  CommandResponse Send(CommandRequest request);
}

public class SerialService : ISerialService
{
  public string[] GetPorts()
  {
     return SerialPort.GetPortNames();
  }

  public CommandResponse Send(CommandRequest request)
  {
    //return request.MathOperation switch
    //{
    //    //"Add" => new CalculationResponse(request.FirstNumber + request.SecondNumber),
    //    //"Subtract" => new CalculationResponse(request.FirstNumber - request.SecondNumber),
    //    //"Multiply" => new CalculationResponse(request.FirstNumber * request.SecondNumber),
    //    //"Divide" => request.SecondNumber != 0
    //    //                    ? new CalculationResponse(request.FirstNumber * 1d / request.SecondNumber)
    //    //                    : throw new DivideByZeroException("Cannot divide by zero."),
    //    _ => throw new ArgumentException($"Unknown operation: {request.MathOperation}"),
    //};
    throw new NotImplementedException();
  }
}
