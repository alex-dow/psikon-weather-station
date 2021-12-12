<script>
import MqttForm from "./MqttForm.svelte";
import WiFiForm from "./WiFiForm.svelte";
import { restarting } from './store.js';

function onRestart() {
  restarting.set(true);
  restart();
  setTimeout(() => {
    window.location.reload();
  }, 60000);
}
</script>


<section>
  <div class="container">
    <button type="button" on:click={onRestart}>Restart Server</button>
    <p>restarting: {$restarting}</p>
    <form>
      <WiFiForm/>
      <MqttForm/>
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