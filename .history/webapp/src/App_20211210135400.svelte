<script>
	export let name = 'world';
</script>


  <section>
    <div class="container">
      <div class="loader" id=""
      <ul class="wifi-list">
        <li>
          God Emperor of Verdun
        </li>
      </ul>
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

</main>

<style>
	main {
		text-align: center;
		padding: 1em;
		max-width: 240px;
		margin: 0 auto;
	}
	h1 {
		color: #ff3e00;
		text-transform: uppercase;
		font-size: 4em;
		font-weight: 100;
	}
	@media (min-width: 640px) {
		main {
			max-width: none;
		}
	}
</style>