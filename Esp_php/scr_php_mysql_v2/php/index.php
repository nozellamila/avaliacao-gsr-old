

<!DOCTYPE html>
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <style>
    body {
      min-width: 310px;
    	max-width: 1280px;
    	height: 500px;
      margin: 0 auto;
    }
    h2 {
      font-family: Arial;
      font-size: 2.5rem;
      text-align: center;
    }
  </style>
  <body>
    <h2>ESP Skin Response Condutance</h2>
    <div id="chart-scr" class="container"></div>
    
    
<?php
echo '<p> value1 </p>';

if ($_SERVER["REQUEST_METHOD"] == "POST")
{
  
  $value1 = $_POST["value1"];
  $script = "<script> var value1 = $value1 </script>";

}
echo '<p>' . $value1 . '</p>'

?>
<script> var value1;</script>
<script>
  alert(value1);
var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'chart-scr' },
  title: { text: 'Skin Condutance Response' },
  series: [{
    showInLegend: false,
    data: value1
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    },
    series: { color: '#059e8a' }
  },
  xAxis: { 
    type: 'datetime',
    categories: reading_time
  },
  yAxis: {
    title: { text: 'Condutance' }
    //title: { text: 'Temperature (Fahrenheit)' }
  },
  credits: { enabled: false }
});

</script>
</body>
</html>