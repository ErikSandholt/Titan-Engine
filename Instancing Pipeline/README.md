# Instancing Pipeline

Instantiating render elements allows for faster rendering as all the information is pushed up to the GPU to write the same element in different positions.

## RenderBatchManager

The batch manager stores the components that it needs to update transform and animation buffers on the GPU side. 

When a component with a render element is edited or it’s state changes (Enable or Disable), it adds and/or removes itself from the batchManager.

When the engine renders each frame it retrieves the BatchData that contains all the entities shared data and draws an amount of entities assuming the buffers are aligned with each batch.

