using Microsoft.AspNetCore.Http.HttpResults;

var builder = WebApplication.CreateBuilder(args);

var app = builder.Build();

app.MapGet("/status", Results<Ok<SmartCardStatusResult>, NotFound> () =>
{
    try
    {
        Console.WriteLine("Get status");
        var hasInsertedCard = SmartCardStatus.SmartCardStatus.IsCardInserted();
        return TypedResults.Ok(new SmartCardStatusResult(hasInsertedCard));
    }
    catch (Exception ex)
    {
        Console.WriteLine(ex.Message);
        return TypedResults.NotFound();
    }
});

app.Run();

public record SmartCardStatusResult(bool cardInserted);
