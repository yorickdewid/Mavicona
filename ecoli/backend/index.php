<?php

/**
 * Ecoli backend - Central package repository
 *
 * @package  Ecoli
 * @author   Yorick de Wid <yorick17@outlook.com>
 */

/*
|--------------------------------------------------------------------------
| Load classes
|--------------------------------------------------------------------------
*/

require __DIR__ . '/Application.php';
require __DIR__ . '/SingletonContract.php';
require __DIR__ . '/Config.php';
require __DIR__ . '/Router.php';
require __DIR__ . '/Log.php';
require __DIR__ . '/Request.php';
require __DIR__ . '/Ecoli.php';

/*
|--------------------------------------------------------------------------
| Create The Application
|--------------------------------------------------------------------------
|
| The first thing we will do is create a new application instance
| which serves as the "glue" for all the components, and is
| the IoC container for the system binding all of the various parts.
|
*/

$app = new Ecoli\Application(Ecoli::class);

$app->singleton([
    Ecoli\Config::class,
    Ecoli\Router::class,
    Ecoli\Log::class,
]);

/*
|--------------------------------------------------------------------------
| Run The Application
|--------------------------------------------------------------------------
|
| Once we have the application, we can handle the incoming request
| through the application core, and send the associated response back to
| the client's browser.
|
*/

$app->loadCore();

$response = $app->handle(
    Ecoli\Request::capture()
);

// $response->send();

$app->terminate();
