/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * License); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Copyright (c) 2021, OPEN AI LAB
 * Author: zpluo@openailab.com
 */

#include "gru_param.h"

#include "api/c_api.h"
#include "graph/tensor.h"
#include "graph/node.h"
#include "graph/graph.h"
#include "module/module.h"
#include "utility/sys_port.h"
#include "utility/vector.h"


static int infer_shape(struct node* node)
{
    struct gru_param* gru_param = (struct gru_param*)(node->op.param_mem);

    struct graph* ir_graph = node->graph;
    struct tensor* input = get_ir_graph_tensor(ir_graph, node->input_tensors[0]);
    struct tensor* output = get_ir_graph_tensor(ir_graph, node->output_tensors[0]);

    int batch_size = input->dims[0];
    int dims[3];

    if (gru_param->mxnet_flag == 1)
    {
        batch_size = input->dims[1];
        dims[0] = input->dims[0];
        dims[1] = batch_size;
        dims[2] = gru_param->hidden_size;
    }
    else
    {
        dims[1] = input->dims[0];
        dims[0] = batch_size;
        dims[2] = gru_param->hidden_size;
    }

    set_ir_tensor_shape(output, dims, 3);

    return 0;
}


static int init_op(struct op* op)
{
    gru_param_t* gru_param = ( gru_param_t* )sys_malloc(sizeof(gru_param_t));

    if (gru_param == NULL)
    {
        return -1;
    }

    gru_param->clip = 0;
    gru_param->output_len = 1;
    gru_param->sequence_len = 1;
    gru_param->input_size = 1;
    gru_param->hidden_size = 1;
    gru_param->has_clip = 0;
    gru_param->has_gate_bias = 0;
    gru_param->has_candidate_bias = 0;
    gru_param->has_init_state = 0;

    op->param_mem = gru_param;
    op->param_size = sizeof(gru_param_t);
    op->same_shape = 0;
    op->infer_shape = infer_shape;

    return 0;
}


static void release_op(struct op* op)
{
    sys_free(op->param_mem);
}


int register_gru_op(void* arg)
{
    struct method m;

    m.version = 1;
    m.init = init_op;
    m.release = release_op;


    return register_op(OP_GRU, OP_GRU_NAME, &m);
}


int unregister_gru_op(void* arg)
{
    return unregister_op(OP_GRU, 1);
}
