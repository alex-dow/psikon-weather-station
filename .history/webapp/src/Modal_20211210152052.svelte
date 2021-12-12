<script>
import { onMount } from "svelte";


export let title = '';
export let open = false;
export let id = '';

onMount(() => {
  if (open === false) {
    document.getElementById(id).style.display = 'none';
  } else {
    document.getElementById(id).style.display = 'block';
  }
});

$: {
  if (open === true) {
    document.getElementById(id).style.display = 'block';
  } else {
    document.getElementById(id).style.display = 'none';
  }
}
</script>
<div class="modal" id={id}>
  <div class="modal-header">
    <slot name="modal-header">
      <h1>{title}</h1>
      <span class="close-button">&times;</span>
    </slot>
  </div>
  <div class="modal-content">
    <slot></slot>
  </div>
</div>

<style>

.modal {
  display: none; /* Hidden by default */
  position: fixed; /* Stay in place */
  z-index: 1; /* Sit on top */
  left: 0;
  top: 0;
  width: 100%; /* Full width */
  height: 100%; /* Full height */
  overflow: auto; /* Enable scroll if needed */
  background-color: rgb(0,0,0); /* Fallback color */
  background-color: rgba(0,0,0,0.4); /* Black w/ opacity */
}

/* Modal Content/Box */
.modal-content {
  background-color: #fefefe;
  margin: 15% auto; /* 15% from the top and centered */
  padding: 20px;
  border: 1px solid #888;
  width: 80%; /* Could be more or less, depending on screen size */
}

</style>