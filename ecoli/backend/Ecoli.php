<?php

use Ecoli\Router;

class Ecoli //extends Core
{
	/**
	 * Append settings to config.
	 *
	 * @return Array
	 */
	public function appendConfig()
	{
		return [
			'name' => 'Ecoli',
			'description' => 'Central package manager',

			'database' => [
				'type' => 'mysql',
				'host' => 'localhost',
				'name' => '',
				'username' => '',
				'password' => '',
			]
		];
	}

	/**
	 * Register routes.
	 *
	 * @return void
	 */
	public function registerRoutes()
	{
		Router::get('repository/main/index.json', 'MainController::getMainRepository');
		Router::get('api/v1/repository', 'ApiController::getMainRepository');
		Router::post('api/v1/package/new', 'ApiController::doNewPackage');
	}

}