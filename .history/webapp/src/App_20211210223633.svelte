<script>
import MqttForm from "./MqttForm.svelte";
import WiFiForm from "./WiFiForm.svelte";
import { restarted } from './store.js';
import { restart } from './api';

function onRestart() {

  await restart();
  restarted.set(true);
  setTimeout(() => {
    window.location.reload();
  }, 60000);
}
</script>

<main>
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
</main>


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