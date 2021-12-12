<script>
	export let name = 'world';
</script>


  <section>
    <div class="container">

    <form>
      <fieldset>
        <legend>WiFi Settings</legend>
        <div class="form-group">
          <label for="wifi_ssid">SSID</label>
          <input id="wifi_ssid"/>
        </div>
        <div class="form-group">
          <label for="wifi_password">Password</label>
          <input id="wifi_password"/>
        </div>
      </fieldset>
      <fieldset>
        <legend>MQTT Settings</legend>
        <div class="form-group">
          <label for="mqtt_host">Hostname or IP Address</label>
          <input id="mqtt_host"/>
        </div>
        <div class="form-group">
          <label for="mqtt_port">Port number</label>
          <input id="mqtt_port" type="number" min="0" max="65536"/>
        </div>
        <div class="form-group">
          <label for="mqtt_username">Username</label>
          <input id="mqtt_username"/>
        </div>
        <div class="form-group">
          <label id="mqtt_password">Password</label>
          <input id="mqtt_password"/>
        </div>
        <div class="form-group">
          <label for="mqtt_id">ID</label>
          <input id="mqtt_id" /><br/>
        </div>
      </fieldset>
    </form>
    </div>
  </section>



<style>
 section {
  min-height: 100vh;
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  background-color: aliceblue;
 }
 .container {
  max-width: 400px;
  width: 90%;
  padding: 20px;
  box-shadow: 0px 0px 20px #00000020;
  border-radius: 8px;
  background-color: white;
 }
</style>